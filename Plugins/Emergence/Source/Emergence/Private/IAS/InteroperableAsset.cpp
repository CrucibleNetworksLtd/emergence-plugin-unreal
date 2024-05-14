#include "IAS/InteroperableAsset.h"

UEmergenceInteroperableAssetSubType* UEmergenceInteroperableAssetLibrary::GetInteroperableAssetSubType(TArray<UEmergenceInteroperableAssetSubType*> Array, TSubclassOf<UEmergenceInteroperableAssetSubType> SubtypeClass, bool& success)
{
	return nullptr;
}

FEmergenceInteroperableAsset UEmergenceInteroperableAssetLibrary::GetTestIA()
{
	return FEmergenceInteroperableAsset(R"({"Metadata":{"Name":"PixelBeast #6210","Description":"","Creator":"Not implemented","Owner":"Not implemented"},"PrimaryAsset":{"MediaType":{"Type":"model","SubType":"vrm"},"AssetLocation":"https://pixelbeasts3d.yoheinakajima.repl.co/beast/6210.vrm","AssetTypeMetadata":{}},"ThumbnailSet":{"SmallThumbnail":"Not implemented","MediumThumbnail":"Not implemented","LargeThumbnail":"Not implemented","IsEmpty":false},"Assets":[{"MediaType":{"Type":"model","SubType":"fbx"},"AssetLocation":"https://pixelbeasts3d.yoheinakajima.repl.co/beast/6210.fbx","AssetTypeMetadata":{}},{"MediaType":{"Type":"model","SubType":"vrm"},"AssetLocation":"https://pixelbeasts3d.yoheinakajima.repl.co/beast/6210.vrm","AssetTypeMetadata":{}},{"MediaType":{"Type":"image","SubType":"png"},"AssetLocation":"https://pixelbeasts3d.yoheinakajima.repl.co/beast/6210.png","AssetTypeMetadata":{}}],"SubTypes":[{"NFTName":"PixelBeast #6210","Description":"","Creator":{"Value":"0xa3622F7312D61DC55F3d889084DFc9Be96516A36"},"Owner":{"Value":"0xa3622F7312D61DC55F3d889084DFc9Be96516A36"},"MintDate":"2024-05-14T10:07:11.5573279+00:00","Attributes":{},"Chain":{"IsTestnet":false,"ChainName":"ethereum"},"TokenNumber":"6210","TokenType":721,"SubTypeName":"NFT"}],"Id":"95673e81-0000-0000-0000-000000000000"})");
}
