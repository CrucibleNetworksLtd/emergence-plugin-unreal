#include "IAS/InteroperableAsset.h"

UEmergenceInteroperableAssetElement* UEmergenceInteroperableAssetLibrary::GetInteroperableAssetElement(TArray<UEmergenceInteroperableAssetElement*> Array, TSubclassOf<UEmergenceInteroperableAssetElement> ElementClass, bool& success)
{
	for (auto Type : Array) {
		if (Type->GetClass() == ElementClass) {
			success = true;
			return Type;
		}
	}
	success = false;
	return nullptr;
}

FEmergenceInteroperableAsset UEmergenceInteroperableAssetLibrary::GetTestIA()
{
	return FEmergenceInteroperableAsset(R"({"Metadata":{"Name":"PixelBeast #6210","Description":"","Creator":"Not implemented","Owner":"Not implemented"},"PrimaryAsset":{"MediaType":{"Type":"model","Element":"vrm"},"AssetLocation":"https://pixelbeasts3d.yoheinakajima.repl.co/beast/6210.vrm","AssetTypeMetadata":{}},"ThumbnailSet":{"SmallThumbnail":"Not implemented","MediumThumbnail":"Not implemented","LargeThumbnail":"Not implemented","IsEmpty":false},"Assets":[{"MediaType":{"Type":"model","Element":"fbx"},"AssetLocation":"https://pixelbeasts3d.yoheinakajima.repl.co/beast/6210.fbx","AssetTypeMetadata":{}},{"MediaType":{"Type":"model","Element":"vrm"},"AssetLocation":"https://pixelbeasts3d.yoheinakajima.repl.co/beast/6210.vrm","AssetTypeMetadata":{}},{"MediaType":{"Type":"image","Element":"png"},"AssetLocation":"https://pixelbeasts3d.yoheinakajima.repl.co/beast/6210.png","AssetTypeMetadata":{}}],"Elements":[{"NFTName":"PixelBeast #6210","Description":"","Creator":{"Value":"0xa3622F7312D61DC55F3d889084DFc9Be96516A36"},"Owner":{"Value":"0xa3622F7312D61DC55F3d889084DFc9Be96516A36"},"MintDate":"2024-05-14T10:07:11.5573279+00:00","Attributes":{},"Chain":{"IsTestnet":false,"ChainName":"ethereum"},"TokenNumber":"6210","TokenType":721,"ElementName":"NFT"}],"Id":"95673e81-0000-0000-0000-000000000000"})");
}
