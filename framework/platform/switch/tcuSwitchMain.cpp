/*-------------------------------------------------------------------------
 * drawElements Quality Program Tester Core
 * ----------------------------------------
 *
 * Nintendo Switch entry point for dEQP.
 * Handles nxlink stdio redirection and default command-line arguments.
 *
 * All Switch-specific code lives here — tcuMain.cpp stays upstream-clean.
 *//*--------------------------------------------------------------------*/

#include "tcuDefs.hpp"
#include "tcuCommandLine.hpp"
#include "tcuPlatform.hpp"
#include "tcuApp.hpp"
#include "tcuResource.hpp"
#include "tcuTestLog.hpp"
#include "tcuTestSessionExecutor.hpp"
#include "deUniquePtr.hpp"
#include "qpDebugOut.h"

#include <cstdio>
#include <switch.h>
#include <unistd.h>

// Implement this in your platform port.
tcu::Platform *createPlatform(void);

/*==========================================================================
 * nxlink stdio redirection
 *==========================================================================*/
static int s_nxlinkSock = -1;

static void initNxLink(void)
{
	if (R_FAILED(socketInitializeDefault()))
		return;

	s_nxlinkSock = nxlinkStdio();
	if (s_nxlinkSock >= 0)
		printf("printf output now goes to nxlink server\n");
	else
		socketExit();
}

static void deinitNxLink(void)
{
	if (s_nxlinkSock >= 0)
	{
		close(s_nxlinkSock);
		socketExit();
		s_nxlinkSock = -1;
	}
}

/*==========================================================================
 * Default caselist in trie/brace notation.
 * Edit this string to add/remove tests, then rebuild.
 *==========================================================================*/
static const char *s_defaultCaselist =
	"--deqp-caselist="
	"{dEQP-GLES2{"
		"info{vendor,renderer,version,shading_language_version,extensions,render_target},"
		"capability{limits{vertex_attribs,varying_vectors,vertex_uniform_vectors,fragment_uniform_vectors,"
			"texture_image_units,vertex_texture_image_units,combined_texture_image_units,"
			"texture_2d_size,texture_cube_size,renderbuffer_size}},"
		"functional{"
			"prerequisite{state_reset,clear_color,read_pixels},"
			"implementation_limits{subpixel_bits,max_texture_size,max_cube_map_texture_size,"
				"max_vertex_attribs,max_vertex_uniform_vectors,max_varying_vectors,"
				"max_combined_texture_image_units,max_vertex_texture_image_units,"
				"max_texture_image_units,max_fragment_uniform_vectors,max_renderbuffer_size,"
				"aliased_point_size_range,aliased_line_width_range,"
				"num_compressed_texture_formats,num_shader_binary_formats,shader_compiler},"
			"color_clear{single_rgb,single_rgba,multiple_rgb,multiple_rgba,long_rgb,long_rgba,"
				"subclears_rgb,subclears_rgba,short_scissored_rgb,scissored_rgb,scissored_rgba,"
				"masked_rgb,masked_rgba,masked_scissored_rgb,masked_scissored_rgba,"
				"complex_rgb,complex_rgba,long_masked_rgb,long_masked_rgba},"
			"depth_stencil_clear{depth,depth_scissored,depth_scissored_masked,"
				"stencil,stencil_scissored,stencil_scissored_masked,"
				"depth_stencil,depth_stencil_scissored,depth_stencil_scissored_masked},"
			"read_pixels{rgba_ubyte_align_1,rgba_ubyte_align_2,rgba_ubyte_align_4,rgba_ubyte_align_8,"
				"choose_align_1,choose_align_2,choose_align_4,choose_align_8},"
			"depth_range{write{default,reverse,zero_to_half,half_to_one,half_to_zero,one_to_half,"
				"third_to_0_8,0_8_to_third,zero_to_zero,half_to_half,one_to_one,"
				"clamp_near,clamp_far,clamp_both},"
				"compare{default,reverse,zero_to_half,half_to_one,half_to_zero,one_to_half,"
				"third_to_0_8,0_8_to_third,zero_to_zero,half_to_half,one_to_one,"
				"clamp_near,clamp_far,clamp_both}},"
			"dither{enabled{gradient_white,gradient_red,gradient_green,gradient_blue,gradient_alpha,"
				"unicolored_quad_white,unicolored_quad_red,unicolored_quad_green,"
				"unicolored_quad_blue,unicolored_quad_alpha},"
				"disabled{gradient_white,gradient_red,gradient_green,gradient_blue,gradient_alpha,"
				"unicolored_quad_white,unicolored_quad_red,unicolored_quad_green,"
				"unicolored_quad_blue,unicolored_quad_alpha}},"
			"debug_marker{supported,random,invalid},"
			"polygon_offset{default_enable,default_displacement_with_units,default_render_with_units,"
				"default_result_depth_clamp,default_render_with_factor,"
				"default_factor_0_slope,default_factor_1_slope,"
				"fixed16_enable,fixed16_displacement_with_units,fixed16_render_with_units,"
				"fixed16_result_depth_clamp,fixed16_render_with_factor,"
				"fixed16_factor_0_slope,fixed16_factor_1_slope},"
			"default_vertex_attrib{float{vertex_attrib_1f,vertex_attrib_2f,vertex_attrib_3f,vertex_attrib_4f,"
				"vertex_attrib_1fv,vertex_attrib_2fv,vertex_attrib_3fv,vertex_attrib_4fv}},"
			"clipping{point{wide_point_clip,wide_point_clip_viewport_center,"
				"wide_point_clip_viewport_corner},"
				"line{wide_line_clip,wide_line_clip_viewport_center,"
				"wide_line_clip_viewport_corner},"
				"triangle_vertex{clip_one,clip_two,clip_three},"
				"polygon,polygon_edge}"
		"}"
	"}}";

int main(int argc, char **argv)
{
	int exitStatus = EXIT_SUCCESS;

	initNxLink();
	printf("[dEQP] === dEQP-GLES2 for Nintendo Switch ===\n");
	printf("[dEQP] argc=%d\n", argc);
	for (int i = 0; i < argc; i++)
		printf("[dEQP] argv[%d]=%s\n", i, argv[i]);
	fflush(stdout);

	// Default arguments for Switch if none provided.
	// Uses --deqp-caselist with inline brace notation (no SD card file needed).
	static const char *switchDefaultArgs[] = {
		"deqp-gles2",
		s_defaultCaselist,
		"--deqp-log-filename=sdmc:/switch/TestResults.qpa",
		"--deqp-archive-dir=sdmc:/switch/",
		"--deqp-log-images=disable",
		"--deqp-watchdog=disable",
	};
	if (argc <= 1)
	{
		printf("[dEQP] No args provided, using defaults\n");
		fflush(stdout);
		argc = 6;
		argv = const_cast<char **>(switchDefaultArgs);
	}

	// Set stdout to line-buffered mode
	setvbuf(stdout, nullptr, _IOLBF, 4 * 1024);

	try
	{
		printf("[dEQP] Step 1: Creating CommandLine...\n"); fflush(stdout);
		tcu::CommandLine cmdLine(argc, argv);

		printf("[dEQP] Step 2: Creating DirArchive (dir=%s)...\n", cmdLine.getArchiveDir()); fflush(stdout);
		tcu::DirArchive archive(cmdLine.getArchiveDir());

		printf("[dEQP] Step 3: Creating TestLog (file=%s)...\n", cmdLine.getLogFileName()); fflush(stdout);
		tcu::TestLog log(cmdLine.getLogFileName(), cmdLine.getLogFlags());

		printf("[dEQP] Step 4: Creating platform...\n"); fflush(stdout);
		de::UniquePtr<tcu::Platform> platform(createPlatform());

		printf("[dEQP] Step 5: Creating App...\n"); fflush(stdout);
		de::UniquePtr<tcu::App> app(new tcu::App(*platform, archive, log, cmdLine));

		printf("[dEQP] Step 6: Entering main loop...\n"); fflush(stdout);

		// Main loop.
		for (;;)
		{
			if (!app->iterate())
			{
				if (cmdLine.getRunMode() == tcu::RUNMODE_EXECUTE &&
					(!app->getResult().isComplete || app->getResult().numFailed))
				{
					exitStatus = EXIT_FAILURE;
				}
				break;
			}
		}
	}
	catch (const std::exception &e)
	{
		printf("[dEQP] EXCEPTION: %s\n", e.what());
		fflush(stdout);
		deinitNxLink();
		return EXIT_FAILURE;
	}

	printf("[dEQP] Exiting cleanly.\n");
	fflush(stdout);
	deinitNxLink();

	return exitStatus;
}
