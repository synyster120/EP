// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "EPFXPreloadLibrary.generated.h"

class UEPFXData;
class UNiagaraSystem;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEPFXPreloadCompleted);

USTRUCT()
struct FEPFXCachedBundle
{
	GENERATED_BODY()

	UPROPERTY(Transient) TObjectPtr<UNiagaraSystem> VFX = nullptr;
	UPROPERTY(Transient) TObjectPtr<USoundBase>    SFX = nullptr;
	UPROPERTY(Transient) TArray<FSoftObjectPath>   LoadedPaths;
};

UCLASS()
class STARBOARDMAP_API UEPFXPreloadLibrary : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void Initialize(FSubsystemCollectionBase& Collection);
	void OnLoadLevel(FGameplayTag& Tag);
	void HandlePostLoadMap(UWorld* LoadedWorld);
	void LoadFXByTag(const FGameplayTag& Tag);
	void LoadFXByTagExec();
	void ClearCacheExceptTag(const FGameplayTag& Tag);

	UPROPERTY()
	FGameplayTag CurrentTag;

	/** 프리로드 완료 이벤트(블루프린트에서 바인딩 가능) */
	UPROPERTY(BlueprintAssignable, Category = "FX|Preload")
	FEPFXPreloadCompleted OnPreloadCompleted;

	/** 캐시에서 가져오기 (없으면 nullptr) */
	UFUNCTION(BlueprintCallable, Category = "FX|Get")
	UNiagaraSystem* GetVFX(FGameplayTag Tag) const;

	/** 캐시에서 가져오기 (없으면 nullptr) */
	UFUNCTION(BlueprintCallable, Category = "FX|Get")
	USoundBase* GetSFX(FGameplayTag Tag) const;

	/** 이미 캐시에 있는지 */
	UFUNCTION(BlueprintCallable, Category = "FX|Get")
	bool IsLoaded(FGameplayTag Tag) const;

	/** 마지막 프리로드가 진행 중인지 */
	UFUNCTION(BlueprintCallable, Category = "FX|State")
	bool IsPreloading() const { return bIsPreloading; }

private:
	/** 하드 캐시 */
	UPROPERTY(Transient)
	TMap<FGameplayTag, FEPFXCachedBundle> FXCache;

	/** 로딩 상태 */
	UPROPERTY(Transient)
	bool bIsPreloading = false;

	/** 현재 로딩 핸들(취소/중복 방지용) */
	TSharedPtr<struct FStreamableHandle> ActiveHandle;
};
