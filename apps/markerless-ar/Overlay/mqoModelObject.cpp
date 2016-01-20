/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//
// Copyright (C) 2012, Takuya MINAGAWA.
// Third party copyrights are property of their respective owners.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//M*/
#include "mqoModelObject.h"

using namespace std;
using namespace cvar;
using namespace cvar::overlay;

mqoModelObject::mqoModelObject(void)
{
	status = UNINIT;
}


mqoModelObject::~mqoModelObject(void)
{
	release();
}

void mqoModelObject::init()
{
	status = INIT;
}

void mqoModelObject::loadModelFile(string filename)
{
	if(status & INIT){
		if(status & LOADED){
			mqoDeleteModel(model);
		}
		model = mqoCreateModel((char*)filename.c_str(),1.0);
		status = status | LOADED;
	}
}

void mqoModelObject::drawModel(int& frame_id)
{
	mqoCallModel(model);
}

void mqoModelObject::release()
{
	if(status & LOADED){
		mqoDeleteModel(model);
//		status = status ^ LOADED;
		status = status - (status & LOADED);
	}
}