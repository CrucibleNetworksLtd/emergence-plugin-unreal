
#pragma once

#include "EdGraphUtilities.h"
#include "EdGraph/EdGraphNode.h"

class FEmergenceWriteMethodGraphPanelNodeFactory: public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override {
		

		if ("Load Contract" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()) {
			Node->SetEnabledState(ENodeEnabledState::Disabled, false);
			Node->SetForceDisplayAsDisabled(true);
			return nullptr;
		}
		
		return nullptr;
	};
};