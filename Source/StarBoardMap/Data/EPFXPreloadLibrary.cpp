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

	//FWorldDelegates::OnPostWorldCreation.AddUObject(this, &UEPFXPreloadLibrary::HandlePostLoadMap);
	//FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UEPFXPreloadLibrary::HandlePostLoadMap);

	if (UWorld* World = GetWorld())
	{
		HandlePostLoadMap(World);
	}


	const UEPFXSettings* Settings = GetDefault<UEPFXSettings>();
	if (!Settings || Settings->DefaultFXData.IsNull())
		return;

	FXData = Settings->DefaultFXData.LoadSynchronous();
}

void UEPFXPreloadLibrary::OnLoadLevel(FGameplayTag& Tag)
{
	UE_LOG(LogTemp, Warning, TEXT("[ERROR FX] OnLoadLevel -- tag : %s"), *Tag.ToString());
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

void UEPFXPreloadLibrary::PreloadFXForLevel(FName LevelName)
{
	FString CleanMapName = LevelName.ToString();
	CleanMapName.RemoveFromEnd(TEXT("Map"), ESearchCase::IgnoreCase);

	// Tag 생성
	const FString TagString = FString::Printf(TEXT("FX.%s"), *CleanMapName);
	UE_LOG(LogTemp, Warning, TEXT("[FX] Level is = %s"), *TagString);
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagString), false); // 실패시 false 반환

	// 태그가 유효한지 체크
	if (Tag.IsValid())
	{
		OnLoadLevel(Tag); // 로드 시작
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tag %s not found in Project Settings!"), *TagString);
		OnPreloadCompleted.Broadcast();
	}
}

void UEPFXPreloadLibrary::LoadFXByTag(const FGameplayTag& Tag)
{
	UE_LOG(LogTemp, Warning, TEXT("[ERROR FX] LoadFXByTag -- tag : %s"), *Tag.ToString());
	const UEPFXSettings* Settings = GetDefault<UEPFXSettings>();
	if (!Settings || Settings->DefaultFXData.IsNull())
		return;

	// Data는 최소 여기서는 Sync로 확보(또는 Data도 async로 포함시키기)
	//UEPFXData* Data = Settings->DefaultFXData.LoadSynchronous();
	if (!FXData) return;

	CurrentTag = Tag;
	const FGameplayTag AllTag = FGameplayTag::RequestGameplayTag(TEXT("FX.All"));
	TArray<FSoftObjectPath> PathsToLoad;

	for (const FEPFXEntry& E : FXData->Entries)
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
	UE_LOG(LogTemp, Warning, TEXT("[ERROR FX] LoadFXByTagExec -- start"));
	FGameplayTag Tag = CurrentTag;

	const UEPFXSettings* Settings = GetDefault<UEPFXSettings>();
	if (!Settings) return;

	//UEPFXData* Data = Settings->DefaultFXData.LoadSynchronous();
	//UEPFXData* Data = Settings->DefaultFXData.Get();
	if (!FXData) return;

	for (const FEPFXEntry& E : FXData->Entries)
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
	UE_LOG(LogTemp, Warning, TEXT("[ERROR FX] LoadFXByTagExec -- for ex"));
	ClearCacheExceptTag(Tag);

	bIsPreloading = false;
	OnPreloadCompleted.Broadcast();

	UE_LOG(LogTemp, Warning, TEXT("[ERROR FX] LoadFXByTagExec -- end - broadcast"));
}

void UEPFXPreloadLibrary::ClearCacheExceptTag(const FGameplayTag& Tag)
{
	UE_LOG(LogTemp, Warning, TEXT("[ERROR FX] LoadFXByTagExec -- ClearCacheExceptTag start"));
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

// 0.0 ~ 1.0 사이의 값 반환
float UEPFXPreloadLibrary::GetFXLoadProgress() const
{
	// 로드 중인 핸들의 진행률 리턴
	return ActiveHandle.IsValid() ? ActiveHandle->GetProgress() : 1.0f;
}