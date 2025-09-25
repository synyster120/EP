// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "EPAsyncLoadHelper.generated.h"

/**
 *      비동기 에셋 로드 helper 함수
 */

UCLASS()
class STARBOARDMAP_API UEPAsyncLoadHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    // UObject 애셋을 비동기적 로드하고, 로드가 완료되면 콜백을 실행하는 범용 함수
    template<typename T>
    static void RequestAsyncLoad(const TSoftObjectPtr<T>& AssetPtr, TFunction<void(T*)> OnLoadedCallback)
    {
        if (AssetPtr.IsNull())
        {
            if (OnLoadedCallback)
            {
                OnLoadedCallback(nullptr);
            }
            return;
        }

        if (T* LoadedAsset = AssetPtr.Get())
        {
            if (OnLoadedCallback)
            {
                OnLoadedCallback(LoadedAsset);
            }
            return;
        }

        UAssetManager& AssetManager = UAssetManager::Get();
        FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();

        StreamableManager.RequestAsyncLoad(AssetPtr.ToSoftObjectPath(),
            [AssetPtr, OnLoadedCallback]()
            {
                if (OnLoadedCallback)
                {
                    OnLoadedCallback(AssetPtr.Get());
                }
            }
        );
    }

    // TSoftClassPtr (클래스) 비동기 로드
    template<typename T>
    static void RequestAsyncLoad(const TSoftClassPtr<T>& ClassPtr, TFunction<void(TSubclassOf<T>)> OnLoadedCallback)
    {
        if (ClassPtr.IsNull())
        {
            if (OnLoadedCallback) { OnLoadedCallback(nullptr); }
            return;
        }

        //  TSubclassOf는 UClass*로 생성
        if (UClass* LoadedClass = ClassPtr.Get())
        {
            if (OnLoadedCallback) { OnLoadedCallback(LoadedClass); }
            return;
        }

        UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(ClassPtr.ToSoftObjectPath(),
            [ClassPtr, OnLoadedCallback]()
            {
                if (OnLoadedCallback) { OnLoadedCallback(ClassPtr.Get()); }
            }
        );
    }
    
};
