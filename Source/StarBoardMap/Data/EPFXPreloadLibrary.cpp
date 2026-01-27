// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/EPFXPreloadLibrary.h"
#include "Data/EPFXSettings.h"

#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"

#include "Data/EPFXData.h"

void UEPFXPreloadLibrary::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FWorldDelegates::OnPostWorldCreation.AddUObject(this, &UEPFXPreloadLibrary::HandlePostLoadMap);

	if (UWorld* World = GetWorld())
	{
		HandlePostLoadMap(World);
	}
}

void UEPFXPreloadLibrary::OnLoadLevel(FGameplayTag& Tag)
{
	bIsPreloading = true;
	LoadFXByTag(Tag);
}

void UEPFXPreloadLibrary::HandlePostLoadMap(UWorld* LoadedWorld)
{
	OnPreloadCompleted.Clear();

	FString CleanMapName = UGameplayStatics::GetCurrentLevelName(LoadedWorld, true);
	CleanMapName.RemoveFromEnd(TEXT("Map"), ESearchCase::IgnoreCase);
	
	const FString TagString = FString::Printf(TEXT("FX.%s"), *CleanMapName);
	UE_LOG(LogTemp, Warning, TEXT("[FX] Level is = %s"), *TagString);
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagString));
	OnLoadLevel(Tag);
}

void UEPFXPreloadLibrary::LoadFXByTag(const FGameplayTag& Tag)
{
	const UEPFXSettings* Settings = GetDefault<UEPFXSettings>();
	if (!Settings || Settings->DefaultFXData.IsNull())
		return;

	// Data는 최소 여기서는 Sync로 확보(또는 Data도 async로 포함시키기)
	UEPFXData* Data = Settings->DefaultFXData.LoadSynchronous();
	if (!Data) return;

	CurrentTag = Tag;
	const FGameplayTag AllTag = FGameplayTag::RequestGameplayTag(TEXT("FX.All"));
	TArray<FSoftObjectPath> PathsToLoad;

	for (const FEPFXEntry& E : Data->Entries)
	{
		if (E.Tag.MatchesTag(AllTag) || !E.Tag.MatchesTag(Tag))
			continue;

		if (FXCache.Contains(E.Tag))
			continue;

		if (!E.VFX.IsNull())
			PathsToLoad.Add(E.VFX.ToSoftObjectPath());

		if (!E.SFX.IsNull())
			PathsToLoad.Add(E.SFX.ToSoftObjectPath());
	}

	if (PathsToLoad.IsEmpty())
	{
		// 로드할 게 없으면 그냥 완료 처리
		bIsPreloading = false;
		OnPreloadCompleted.Broadcast();
		return;
	}

	bIsPreloading = true;

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	ActiveHandle = Streamable.RequestAsyncLoad(
		PathsToLoad,
		FStreamableDelegate::CreateUObject(this, &UEPFXPreloadLibrary::LoadFXByTagExec)
	);
}

void UEPFXPreloadLibrary::LoadFXByTagExec()
{
	FGameplayTag Tag = CurrentTag;

	const UEPFXSettings* Settings = GetDefault<UEPFXSettings>();
	if (!Settings) return;

	UEPFXData* Data = Settings->DefaultFXData.Get();
	if (!Data) return;

	for (const FEPFXEntry& E : Data->Entries)
	{
		if (!E.Tag.MatchesTag(Tag))
			continue;

		if (FXCache.Contains(E.Tag))
			continue;

		FEPFXCachedBundle Bundle;
		Bundle.VFX = E.VFX.IsNull() ? nullptr : Cast<UNiagaraSystem>(E.VFX.Get());
		Bundle.SFX = E.SFX.IsNull() ? nullptr : Cast<USoundBase>(E.SFX.Get());

		UE_LOG(LogTemp, Warning, TEXT("[FXCache] Cached Tag: %s"), *E.Tag.ToString());
		FXCache.Add(E.Tag, Bundle);
	}
	ClearCacheExceptTag(Tag);

	bIsPreloading = false;
	OnPreloadCompleted.Broadcast();
}

void UEPFXPreloadLibrary::ClearCacheExceptTag(const FGameplayTag& Tag)
{
	const FGameplayTag AllTag = FGameplayTag::RequestGameplayTag(TEXT("FX.All"));

	TArray<FGameplayTag> TagsToRemove;

	for (const auto& Pair : FXCache)
	{
		const FGameplayTag& CachedTag = Pair.Key;

		// FX.ALL 은 절대 제거 안 함
		if (CachedTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("FX.All"))))
		{
			continue;
		}

		// 현재 레벨 태그에 속하지 않으면 제거 대상
		if (!CachedTag.MatchesTag(Tag))
		{
			TagsToRemove.Add(CachedTag);
		}
	}

	for (const FGameplayTag& RemovingTag : TagsToRemove)
	{
		FXCache.Remove(RemovingTag);
	}
}

bool UEPFXPreloadLibrary::IsLoaded(FGameplayTag Tag) const
{
	return FXCache.Contains(Tag);
}

UNiagaraSystem* UEPFXPreloadLibrary::GetVFX(FGameplayTag Tag) const
{
	if (const FEPFXCachedBundle* Bundle = FXCache.Find(Tag))
	{
		return Bundle->VFX;
	}
	return nullptr;
}

USoundBase* UEPFXPreloadLibrary::GetSFX(FGameplayTag Tag) const
{
	if (const FEPFXCachedBundle* Bundle = FXCache.Find(Tag))
	{
		return Bundle->SFX;
	}
	return nullptr;
}