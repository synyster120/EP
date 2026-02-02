// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/EPFXData.h"

#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"

static FString TagToStringSafe(const FGameplayTag& Tag)
{
	return Tag.IsValid() ? Tag.ToString() : FString();
}

void UEPFXData::BuildIndexInternal() const
{
	TagToIndex.Reset();
	bIndexBuilt = true;

	for (int32 i = 0; i < Entries.Num(); ++i)
	{
		const FGameplayTag& Tag = Entries[i].Tag;
		if (!Tag.IsValid()) continue;

		// 중복이면 첫 번째만 유지
		if (!TagToIndex.Contains(Tag))
		{
			TagToIndex.Add(Tag, i);
		}
	}
}

bool UEPFXData::FindEntry(FGameplayTag Tag, FEPFXEntry& OutEntry) const
{
	if (!Tag.IsValid()) return false;

	if (!bIndexBuilt)
	{
		BuildIndexInternal();
	}

	if (const int32* Idx = TagToIndex.Find(Tag))
	{
		if (Entries.IsValidIndex(*Idx))
		{
			OutEntry = Entries[*Idx]; // 구조체 복사(작아서 문제 없음)
			return true;
		}
	}
	return false;
}

const FEPFXEntry* UEPFXData::FindEntryPtr(FGameplayTag Tag) const
{
	if (!Tag.IsValid()) return nullptr;

	if (!bIndexBuilt)
	{
		BuildIndexInternal();
	}

	if (const int32* Idx = TagToIndex.Find(Tag))
	{
		return Entries.IsValidIndex(*Idx) ? &Entries[*Idx] : nullptr;
	}
	return nullptr;
}

FName UEPFXData::ResolveCategoryFromTag(FGameplayTag Tag) const
{
	// Expect: FX.<Category>....
	// e.g. FX.Circus.Fire.Explosion => "Circus"
	if (!Tag.IsValid()) return NAME_None;

	const FString TagStr = Tag.ToString();
	TArray<FString> Parts;
	TagStr.ParseIntoArray(Parts, TEXT("."), true);

	if (Parts.Num() < 2) return NAME_None;
	if (!Parts[0].Equals(TEXT("FX"), ESearchCase::IgnoreCase)) return NAME_None;

	const FString& Cat = Parts[1];
	return Cat.IsEmpty() ? NAME_None : FName(*Cat);
}

void UEPFXData::WarnDuplicateTags() const
{
	TMap<FGameplayTag, int32> FirstSeenIndex;

	for (int32 i = 0; i < Entries.Num(); ++i)
	{
		const FGameplayTag& Tag = Entries[i].Tag;
		if (!Tag.IsValid())
		{
			continue;
		}

		if (const int32* FirstIdx = FirstSeenIndex.Find(Tag))
		{
			UE_LOG(LogTemp, Warning, TEXT("[EPFXData] Duplicate Tag found: %s (first=%d, dup=%d)"),
				*Tag.ToString(), *FirstIdx, i);
		}
		else
		{
			FirstSeenIndex.Add(Tag, i);
		}
	}
}

void UEPFXData::SortEntriesByTagString()
{
	Entries.Sort([](const FEPFXEntry& A, const FEPFXEntry& B)
		{
			const FString AS = TagToStringSafe(A.Tag);
			const FString BS = TagToStringSafe(B.Tag);
			return AS < BS;
		});
}
	
void UEPFXData::BuildIndex() const
{
	BuildIndexInternal();
}

void UEPFXData::OrganizeNow()
{
	// 1) Category 자동 채움
	for (FEPFXEntry& E : Entries)
	{
		E.Category = ResolveCategoryFromTag(E.Tag);
	}
	WarnDuplicateTags();

	// 2) Tag 기준 정렬 (Entries 배열 자체가 바뀜)
	SortEntriesByTagString();

	// 3) 인덱스 갱신
	InvalidateIndex();
	BuildIndexInternal();
}