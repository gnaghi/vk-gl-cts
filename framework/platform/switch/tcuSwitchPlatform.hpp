/*
 * VK-GL-CTS Nintendo Switch Platform
 *
 * Platform layer for running dEQP conformance tests on Nintendo Switch
 * using SwitchGLES (OpenGL ES 2.0 + EGL 1.4 over deko3d).
 */

#ifndef _TCUSWITCHPLATFORM_HPP
#define _TCUSWITCHPLATFORM_HPP

#include "tcuDefs.hpp"
#include "tcuPlatform.hpp"
#include "gluPlatform.hpp"
#include "egluPlatform.hpp"

namespace tcu
{
namespace nx
{

class Platform : public tcu::Platform, private glu::Platform, private eglu::Platform
{
public:
	Platform(void);
	virtual ~Platform(void);

	virtual const glu::Platform&  getGLPlatform(void) const  { return static_cast<const glu::Platform&>(*this); }
	virtual const eglu::Platform& getEGLPlatform(void) const { return static_cast<const eglu::Platform&>(*this); }

	virtual bool processEvents(void);
	virtual void getMemoryLimits(tcu::PlatformMemoryLimits& limits) const;
};

} // namespace nx
} // namespace tcu

#endif // _TCUSWITCHPLATFORM_HPP
