#include "InputSystem.h"

namespace PbrRtV2
{
void InputSystem::SetSurfelDebugEnabled(bool enabled)
{
	m_SurfelDebugEnabled = enabled;
	if (!m_SurfelDebugEnabled)
	{
		m_SurfelDebugMode = SurfelDebugMode::Disabled;
	}
	else if (m_SurfelDebugMode == SurfelDebugMode::Disabled)
	{
		m_SurfelDebugMode = SurfelDebugMode::Points;
	}
}

bool InputSystem::IsSurfelDebugEnabled() const
{
	return m_SurfelDebugEnabled;
}

void InputSystem::CycleSurfelDebugMode()
{
	if (!m_SurfelDebugEnabled)
	{
		m_SurfelDebugMode = SurfelDebugMode::Disabled;
		return;
	}

	switch (m_SurfelDebugMode)
	{
	case SurfelDebugMode::Disabled:
		m_SurfelDebugMode = SurfelDebugMode::Points;
		break;
	case SurfelDebugMode::Points:
		m_SurfelDebugMode = SurfelDebugMode::Normals;
		break;
	case SurfelDebugMode::Normals:
		m_SurfelDebugMode = SurfelDebugMode::Density;
		break;
	case SurfelDebugMode::Density:
		m_SurfelDebugMode = SurfelDebugMode::Points;
		break;
	}
}

SurfelDebugMode InputSystem::GetSurfelDebugMode() const
{
	return m_SurfelDebugMode;
}

void InputSystem::ProcessSurfelDebugInput(bool resetKeyDown, bool shiftKeyDown)
{
	if (resetKeyDown && !m_ResetKeyWasDown)
	{
		if (shiftKeyDown)
		{
			if (!m_SurfelDebugEnabled)
			{
				SetSurfelDebugEnabled(true);
			}
			else
			{
				CycleSurfelDebugMode();
			}
		}
		else
		{
			SetSurfelDebugEnabled(!m_SurfelDebugEnabled);
		}
	}

	m_ResetKeyWasDown = resetKeyDown;
}
} // namespace PbrRtV2
