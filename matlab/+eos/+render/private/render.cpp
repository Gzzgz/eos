/*
 * eos - A 3D Morphable Model fitting library written in modern C++11/14.
 *
 * File: matlab/+eos/+render/private/render.cpp
 *
 * Copyright 2017 Patrik Huber
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "eos/core/Mesh.hpp"
#include "eos/fitting/RenderingParameters.hpp"
#include "eos/render/texture_extraction.hpp"

#include "mexplus_opencv.hpp"
#include "mexplus_eos_types.hpp"

#include "mexplus.h"

#include "opencv2/core/core.hpp"

#include "mex.h"

using namespace eos;
using namespace mexplus;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	// Check for proper number of input and output arguments:
	if (nrhs != 5) {
		mexErrMsgIdAndTxt("eos:render:nargin", "extract_texture requires 5 input arguments.");
	}
	if (nlhs != 1) {
		mexErrMsgIdAndTxt("eos:render:nargout", "extract_texture returns one output argument.");
	}

	InputArguments input(nrhs, prhs, 5);
	const auto mesh = input.get<core::Mesh>(0);
	const auto rendering_params = input.get<fitting::RenderingParameters>(1);
	const auto image = input.get<cv::Mat>(2);
	const auto compute_view_angle = input.get<bool>(3); // default: false
	const auto isomap_resolution = input.get<int>(4); // default: 512

	// We expect to be given a RGB image. Let's convert it to BGR for OpenCV.
	// Actually, it doesn't matter at all for the texture extraction - just keep it!
	//cv::Mat image_as_bgr;// = image.clone();
	//cv::cvtColor(image, image_as_bgr, cv::COLOR_RGB2BGR);

	// Now do the actual extraction:
	const cv::Mat affine_from_ortho = fitting::get_3x4_affine_camera_matrix(rendering_params, image.cols, image.rows);
	const cv::Mat isomap = render::extract_texture(mesh, affine_from_ortho, image, compute_view_angle, render::TextureInterpolation::NearestNeighbour, isomap_resolution);
	
	// Return the extracted texture-map:
	OutputArguments output(nlhs, plhs, 1);
	output.set(0, isomap);
};
