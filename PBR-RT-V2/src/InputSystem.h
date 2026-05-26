#pragma once

#include "SurfelRendering/SurfelDebugView.h"

namespace PbrRtV2
{
class InputSystem
{
public:
	void SetSurfelDebugEnabled(bool enabled);
	bool IsSurfelDebugEnabled() const;

	void CycleSurfelDebugMode();
	SurfelDebugMode GetSurfelDebugMode() const;

	void ProcessSurfelDebugInput(bool resetKeyDown, bool shiftKeyDown);

private:
	bool m_SurfelDebugEnabled = false;
	SurfelDebugMode m_SurfelDebugMode = SurfelDebugMode::Disabled;
	bool m_ResetKeyWasDown = false;
};
} // namespace PbrRtV2
