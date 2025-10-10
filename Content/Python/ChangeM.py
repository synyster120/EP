import unreal

def find_material(mat_name: str) -> unreal.MaterialInterface:
    SEARCH_PATH = "/Game/AssetStatic"
    for path in unreal.EditorAssetLibrary.list_assets(
        SEARCH_PATH, recursive=True, include_folder=False
    ):
        if path.endswith(f"{mat_name}.{mat_name}"):
            return unreal.EditorAssetLibrary.load_asset(path)
    return None

def remap_selected_meshes():
    any_replaced = False

    # Content Browser에서 선택된 에셋 순회
    for asset in unreal.EditorUtilityLibrary.get_selected_assets():
        if not isinstance(asset, unreal.StaticMesh):
            continue
        mesh = asset

        # 1) static_materials 리스트 가져오기
        slot_list = mesh.static_materials  # [StaticMaterial, StaticMaterial, ...]

        # 2) 각 슬롯 검사 및 교체
        for idx, slot in enumerate(slot_list):
            orig_mat  = slot.material_interface
            orig_name = orig_mat.get_name()
            if not orig_name.startswith("M_"):
                continue

            new_name = "MI_" + orig_name[2:]
            new_mat  = find_material(new_name)
            if new_mat:
                slot.material_interface = new_mat
                slot_list[idx]         = slot
                any_replaced           = True
                unreal.log(f"[Remap] {mesh.get_name()}: {orig_name} → {new_name}")

        # 3) 변경된 슬롯 리스트를 에셋에 적용 & 바로 저장
        mesh.set_editor_property("static_materials", slot_list)
        # 수정: 경로 문자열 대신 UObject(mesh) 자체를 넘깁니다
        unreal.EditorAssetLibrary.save_loaded_asset(mesh)

    # 4) 최종 완료 메시지
    if any_replaced:
        unreal.log("✅ 머티리얼이 교체되었습니다.")
    else:
        unreal.log_warning("⚠️ 교체된 머티리얼이 없습니다.")

if __name__ == "__main__":
    remap_selected_meshes()
