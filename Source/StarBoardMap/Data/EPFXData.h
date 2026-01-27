// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EPFXData.generated.h"

class UNiagaraSystem;
class USoundBase;

USTRUCT(BlueprintType)
struct FEPFXEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UNiagaraSystem> VFX;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USoundBase> SFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX")
	FName Category = NAME_None;
};

UCLASS()
class STARBOARDMAP_API UEPFXData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	TArray<FEPFXEntry> Entries;

	/** If true, AutoFillCategoryAndSort() will also sort by Tag string */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|Auto")
	bool bAutoOrganizeOnEdit = false;

	/** Build transient Tag->Index map (call after edits or on first use) */
	UFUNCTION(BlueprintCallable, Category = "FX")
	void BuildIndex() const;

	/** (에디터 버튼) Tag 기반으로 Category 채우고, Tag 기준으로 정렬 */
	UFUNCTION(CallInEditor, Category = "FX|Editor")
	void OrganizeNow();

	/** Find entry by tag. Returns nullptr if not found. */
	UFUNCTION(BlueprintCallable, Category = "FX")
	bool FindEntry(FGameplayTag Tag, FEPFXEntry& OutEntry) const;

	// C++ 전용(포인터 반환 OK) - BP 노출 안 함
	const FEPFXEntry* FindEntryPtr(FGameplayTag Tag) const;

private:
	/** transient index for fast lookup */
	UPROPERTY(Transient)
	mutable TMap<FGameplayTag, int32> TagToIndex;

	UPROPERTY(Transient)
	mutable bool bIndexBuilt = false;

	// Internal helpers
	FName ResolveCategoryFromTag(FGameplayTag Tag) const;
	void WarnDuplicateTags() const;
	void SortEntriesByTagString();
	void InvalidateIndex() { bIndexBuilt = false; TagToIndex.Reset(); }
	void BuildIndexInternal() const;
};
