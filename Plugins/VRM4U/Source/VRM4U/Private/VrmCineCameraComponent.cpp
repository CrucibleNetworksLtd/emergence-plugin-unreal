// VRM4U Copyright (c) 2021-2022 Haruyoshi Yamamoto. This software is released under the MIT License.

#include "VrmCineCameraComponent.h"
#include "Misc/EngineVersionComparison.h"

void UVrmCineCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) {
	Super::GetCameraView(DeltaTime, DesiredView);
	DesiredView.OffCenterProjectionOffset = OffCenterProjectionOffset;
}
