#include "../../_common/vOpenCV/OpenCV.h"
#include "../../_common/vOpenCV/BlobTracker.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

using namespace cv;

VideoInput input;
char info[256];

#define MAIN_WINDOW "Main"

int thresh_value = 2;const int THRESH_VALUE = 155;
int x_delta = 10;const int X_DELTA = 20;
int similar_value = 4;const int SIMILAR_VALUE = 10; 
int dilate_value = 0;const int DILATE_VALUE = 1;
int erode_value = 0;const int ERODE_VALUE = 1; 

Ptr<IplImage> raw;
Ptr<IplImage> frame;
Ptr<IplImage> mask;
Ptr<IplImage> bigMask;
Ptr<IplImage> zoomImage;

cv::Size outSize;

typedef std::vector<vBlob> BlobArray;
BlobArray blobs;
BlobArray bigBlobs;
int n_bigBlobs;

struct vBlobFont;
std::vector<vBlobFont*> selected;
vBlobFont* getBlobByAt(int x, int y);

char* g_typeFileName = NULL;

bool show_box = true;
#ifdef _DEBUG
bool show_outline = true;
#else
bool show_outline = false;
#endif // _DEBUG

char g_inputFilename[_MAX_FNAME];
char g_outputFileName[_MAX_FNAME];

vector<int> characters;

int scaleFactor = 1;
int lineHeight = 0;

int getIndex(int y)
{
	for (int i=0;i<n_bigBlobs;i++)
	{
		if (y < bigBlobs[i].box.y+bigBlobs[i].box.height)
			return i;
	}
	return -1;
}

struct vBlobFont : public vBlob
{
	vBlobFont(const vBlob& b):vBlob(b)
	{		
		idx_y = getIndex(center.y);
		offset_x = 0;
		bias_param = 0;
		offset_y = box.y - idx_y * lineHeight;
	}

	bool operator<(const vBlobFont& other) const 
	{//sorted by Y-coord first then X-coord
		return (idx_y < other.idx_y) || 
			( (idx_y == other.idx_y) && (center.x < other.center.x)); 
	}

	void drawBoxBiased(IplImage* image, const CvScalar& clr)
	{
		Rect rect = box;
		rect.y += bias_param;
		vDrawRect(image, rect, clr);
	}

	int idx_y;
	int offset_x;
	int offset_y;
	int bias_param;//adjust it manually, via MouseWheel
};

std::vector<vBlobFont> tempFonts;
std::vector<vBlobFont> finalFonts;

vBlobFont* getBlobByAt(int x, int y)
{
	int n = finalFonts.size();
	for (int i=0;i<n;i++)
	{
		Rect rect = finalFonts[i].box;
		rect.x -= 1;
		rect.y -= 1;
		rect.width += 1;
		rect.height += 1;
		rect.y += finalFonts[i].bias_param;
		if (rect.contains(Point(x,y)))
			return &finalFonts[i];
	}
	return NULL;
}

void on_update(int p = 0);
void on_paint();

void onMouse(int Event,int x,int y,int flags,void* param )
{ 
	if (x < 0 || x > 2000) 
		x = 0;
	else if (x > outSize.width ) x = outSize.width;
	if (y < 0 || y > 2000) y = 0;
	else if (y > outSize.height ) y = outSize.height;

	if( Event == CV_EVENT_LBUTTONUP )
	{
		if (!(flags & CV_EVENT_FLAG_SHIFTKEY))
			selected.clear();
		vBlobFont* one = getBlobByAt(x,y);
		if (one)
		{
			bool notExist = true;
			for (int k=0;k<selected.size();k++)
			{
				if (selected[k] == one)
				{
					notExist = false;
					break;
				}				
			}
			if (notExist)
				selected.push_back(one);
		}

		on_paint();
	}
}

void on_paint()
{
	if (scaleFactor != 1)
		cvResize(raw, frame);
	else
		cvCopy(raw, frame);

	if (show_outline)
	{
		for (int i=0;i<n_bigBlobs;i++)
		{
			vDrawRect(frame, bigBlobs[i].box, vDefaultColor(i));
			cvLine(frame, cvPoint(0, (i+1)*lineHeight), cvPoint(outSize.width-1, (i+1)*lineHeight), 
				CV_WHITE, 1);
		}			
	}

	int n_finalFonts = finalFonts.size();
#ifdef _DEBUG
	static int counter = 0;	
	n_finalFonts = min(counter, n_finalFonts-1);
	counter++;

	for (int i=0;i<n_finalFonts;i++)
	{
		vBlob& obj = finalFonts[i];
		vDrawRect(frame, obj.box, CV_RGB(255, 255, 255));
	}
	vBlob& obj = finalFonts[n_finalFonts];
	n_finalFonts++;
	vDrawRect(frame, obj.box, CV_RGB(255, 0, 0));
#else
	if (show_box)
	{
		for (int i=0;i<n_finalFonts;i++)
		{
			vBlobFont& obj = finalFonts[i];
			const Rect& box = obj.box;
			CvScalar clr = obj.bias_param == 0 ? CV_WHITE : CV_RGB(220,220,50);
			obj.drawBoxBiased(frame, clr);
			//	vDrawRect(frame, box, );
		}
	}
#endif

	const int INFO_HEIGHT = 20;

	int infoHeight = (bigBlobs[n_bigBlobs-1].box.y+bigBlobs[n_bigBlobs-1].box.height+outSize.height)/2 - INFO_HEIGHT;

	bool isSizeMatch = true;
	if (characters.size() != finalFonts.size())
	{
		sprintf(info, "found chars: #%d. doesn't match \"%s\".", 
			finalFonts.size(), g_typeFileName);
		isSizeMatch = false;
	}
	else
		sprintf(info, "found chars: #%d. Thanks GOD it matches!!!!!!",	n_finalFonts);

	vDrawText(frame, 30,infoHeight,info, CV_GREEN);

	sprintf(info, "click to select the char, then [W] move up, [S] move down, [X] move to original.");
	infoHeight += INFO_HEIGHT;vDrawText(frame, 30,infoHeight,	info, CV_GREEN);

	if (isSizeMatch)
		sprintf(info, "\"%s\" is auto saved. Press ESC to exit.", g_outputFileName);
	else
		sprintf(info, "no file be saved until found chars matches.");
	infoHeight += INFO_HEIGHT;vDrawText(frame, 30,infoHeight,	info, CV_GREEN);

	for (int i=0;i<selected.size();i++)
	{
		vBlobFont* one = selected[i];
		if (one)
			one->drawBoxBiased(frame, CV_GREEN);
	}

	FILE* fout = fopen(g_outputFileName, "w");
	if (fout == NULL)
		return;

	fprintf(fout, "common lineHeight=%d scaleW=%d scaleH=%d\n", 
		lineHeight/scaleFactor,	input._size.width, input._size.height);
	fprintf(fout, "page file=""%s""\n", g_inputFilename);
	fprintf(fout, "chars count=%d\n", n_finalFonts);
	for (int i=0;i<n_finalFonts;i++)
	{
		vBlobFont& obj = finalFonts[i];
		const Rect& box = obj.box;

		fprintf(fout, "char id=%d x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d\n",
			isSizeMatch ? characters[i] : 77,
			box.x/scaleFactor, box.y/scaleFactor,
			box.width/scaleFactor, box.height/scaleFactor,
			obj.offset_x/scaleFactor, (obj.offset_y+obj.bias_param)/scaleFactor,
			box.width/scaleFactor+2
			);
	}

	printf("FILE %s SAVED\n", g_outputFileName);
	fclose(fout);

	show_image(frame);
}
void on_update(int )
{
	selected.clear();

	if (scaleFactor != 1)
		cvResize(raw, frame);
	else
		cvCopy(raw, frame);

	vGrayScale(frame, mask);

	if (erode_value > 0)
		vErode(mask, erode_value);
	if (dilate_value > 0)
		vDilate(mask, dilate_value);
	vThresh(mask, thresh_value);
	cvCopy(mask, bigMask);

#ifdef _DEBUG
	show_image(mask);
#endif // _DEBUG

	//[first phase blobs]
	vFindBlobs(mask, blobs, 1);	

	int n_blobs = blobs.size();
	//[draw lines to differ bigBlobs]
	for (int i=0;i<n_blobs;i++)
	{
		vBlob& obj = blobs[i];
		obj.center.x = obj.box.x + obj.box.width/2;
		obj.center.y = obj.box.y + obj.box.height/2;
		cvLine(bigMask, cvPoint(0, obj.center.y), cvPoint(outSize.width-1, obj.center.y), CV_WHITE, 3);
	}

	vOpen(bigMask, 2);
#ifdef _DEBUG
	show_image(bigMask);
#endif // _DEBUG

	vFindBlobs(bigMask, bigBlobs, 1000);

	BlobArray::const_iterator it=bigBlobs.begin();
	while (it != bigBlobs.end())
	{
		if (it->isHole || it->box.width < outSize.width*0.8)
			it = bigBlobs.erase(it);
		else
			it++;
	}	
	std::sort(bigBlobs.begin(), bigBlobs.end());//first phase-sort on bigBlobs

	n_bigBlobs = bigBlobs.size();

	if (n_bigBlobs == 0)
		return;

	lineHeight = bigBlobs[n_bigBlobs-1].box.y + bigBlobs[n_bigBlobs-1].box.height- bigBlobs[0].box.x;
	lineHeight = lineHeight/n_bigBlobs + 1;

	for (int i=0;i<n_bigBlobs;i++)
	{		
		if (i > 0)
		{
			vBlob& prev = bigBlobs[i-1];
			vBlob& obj = bigBlobs[i];
			if (prev.box.height < lineHeight*0.5)
			{//	if too thin, merge to next line
				obj.boxMerge(prev);
				prev.isHole = true;
			}
		}
	}
	it=bigBlobs.begin();
	while (it != bigBlobs.end())
	{
		if (it->isHole)
			it = bigBlobs.erase(it);
		else
			it++;
	}
	std::sort(bigBlobs.begin(), bigBlobs.end());//second phase-sort on bigBlobs

	n_bigBlobs = bigBlobs.size();
	lineHeight = bigBlobs[n_bigBlobs-1].box.y + bigBlobs[n_bigBlobs-1].box.height- bigBlobs[0].box.x;
	lineHeight = lineHeight/n_bigBlobs + 1;

	//[second phase blobs]
	tempFonts.clear();
	tempFonts.reserve(n_blobs);

	for (int i=0;i<n_blobs;i++)
	{
		vBlob& obj = blobs[i];
		if (!obj.isHole)
		{
			obj.pts.clear();//for optimization
			tempFonts.push_back(obj);
		}
	}
	int n_tempFonts = tempFonts.size();
	finalFonts.clear();
	finalFonts.reserve(n_blobs);

	std::sort(tempFonts.begin(), tempFonts.end());//sort tempFonts

	for (int i=0;i<n_tempFonts;i++)
	{
		vBlobFont& obj = tempFonts[i];

		if (i == 0 
			|| (i >= n_tempFonts - 4))
		{
			finalFonts.push_back(obj);
		}
		else
		{
			vBlobFont& prev = finalFonts.back();
			if (vTestRectHitRect(obj.box, prev.box)
				||(obj.idx_y == prev.idx_y && abs(obj.center.x - prev.center.x) < x_delta && obj.similar(prev,similar_value))
				||( obj.center.x > prev.box.x && obj.center.x < prev.box.x+prev.box.width)
				||( prev.center.x > obj.box.x && prev.center.x < obj.box.x+obj.box.width)
				)//	merge close blobs
			{
				prev.boxMerge(obj);
			}
			else
			{
				finalFonts.push_back(obj);
			}
		}
	}

	int n_finalFonts = finalFonts.size();

	{//deal with three dots ---> ellipsis
		vBlobFont* dots[3] = {
			&finalFonts[n_finalFonts-4], 
			&finalFonts[n_finalFonts-3],
			&finalFonts[n_finalFonts-2]
		};

		if (dots[0]->similar(*dots[1], similar_value) && 
			dots[1]->similar(*dots[2], similar_value) && 
			dots[0]->box.height < lineHeight * 0.5 &&
			dots[1]->box.height < lineHeight * 0.5 && 
			dots[2]->box.height < lineHeight * 0.5)
		{
			dots[0]->boxMerge(*dots[2]);
			// 			finalFonts.erase(finalFonts.begin()+n_finalFonts-2);
			// 			finalFonts.erase(finalFonts.begin()+n_finalFonts-3);
			// /*			finalFonts.erase(finalFonts.begin()+n_finalFonts-2);*/
			std::swap(finalFonts[n_finalFonts-3], finalFonts[n_finalFonts-1]);
			finalFonts.pop_back();
			finalFonts.pop_back();
		}
	}//deal with three dots ---> ellipsis

	on_paint();

}

int main(int argc, char** argv )
{
	if (argc != 3)
	{
		printf("[USAGE] FontCutter.exe texture_image character_order_file\n");
		goto _fail;
	}

	g_typeFileName = argv[2];
	FILE* fin = fopen(g_typeFileName,"r");
	if (!fin)
	{
		printf("character_order_file is missing or wrong\n");
		goto _fail;
	}

	printf("seems alright, ready to open image\n");
	char* image = argv[1];
	if (input.init(image))
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath(argv[1], drive, dir, fname, ext);
		sprintf(g_inputFilename, "\"%s%s\"", fname, ext);
		sprintf(g_outputFileName,"%s.fnt",fname);

		int chr;
		while (fscanf(fin, "%d", &chr) != EOF)
		{
			characters.push_back(chr);
		}

		IplImage* _raw = input.get_frame(); 
		if (!_raw)
		{
			printf("error @ input.get_frame()");
			return -1;
		}

		cvNamedWindow(MAIN_WINDOW);

		cvCreateTrackbar("Small Thin",MAIN_WINDOW,&thresh_value, THRESH_VALUE, on_update);
		cvCreateTrackbar("Big Thin",MAIN_WINDOW,&erode_value, ERODE_VALUE, on_update);
		cvCreateTrackbar("X Delta",MAIN_WINDOW,&x_delta, X_DELTA, on_update);
/*		cvCreateTrackbar("Similar",MAIN_WINDOW,&similar_value, SIMILAR_VALUE, on_update);*/
		//	cvCreateTrackbar("Fatter",MAIN_WINDOW,&dilate_value, DILATE_VALUE, on_update);

		cvShowImage(MAIN_WINDOW, _raw);

		cvNamedWindow("frame");
		cvSetMouseCallback("frame", onMouse);

		cv::Size size = input._size;		
		if (size.width < 600)
			scaleFactor = 2;
		outSize = input._size*scaleFactor;
		raw = cvCloneImage(_raw);
		frame = cvCreateImage(outSize, 8, 3);
		mask = cvCreateImage(outSize, 8, 1);
		bigMask = cvCreateImage(outSize, 8, 1);

		on_update();

		bool loop = true;


		while (loop)
		{
			int key = cvWaitKey(0);

			bool do_paint = false;
			bool do_update = false;

			switch(key)
			{
			case VK_ESCAPE:
				{
					loop = false;					
				}break;
			case VK_SPACE:
				{
					do_update = true;
				}break;
			case 'n':
				{
					show_outline = !show_outline;
					do_paint = true;
				}break;
			case VK_TAB:
				{
					show_box = !show_box;
					do_paint = true;
				}break;
			case 'w':
				{
					for (int i=0;i<selected.size();i++)
						selected[i]->bias_param -= 1;
					do_paint = true;
				}break;
			case 's':
				{
					for (int i=0;i<selected.size();i++)
						selected[i]->bias_param += 1;
					do_paint = true;
				}break;
			case 'x':
				{
					for (int i=0;i<selected.size();i++)
						selected[i]->bias_param = 0;
					do_paint = true;
				}break;
			case 'g':
				{
					int n = selected.size();
					if (n >= 2)
					{
						vBlobFont* first = selected[0];
						bool hasSameIdxY = true;
						for (int i=1;i<selected.size();i++)
						{
							if (selected[i]->idx_y != first->idx_y)
								hasSameIdxY = false;							
						}
						if (hasSameIdxY)
						{
							for (int i=1;i<selected.size();i++)
							{
								vBlobFont* follower = selected[i];
								first->boxMerge(*selected[i]);
							}
							for (int i=1;i<selected.size();i++)
							{
								vector<vBlobFont>::const_iterator it = finalFonts.begin();
								while (it != finalFonts.end())
								{
									const vBlobFont* one_font = &(*it);
									if (one_font == selected[i])
									{
										it = finalFonts.erase(it);
										break;
									}
									else
										it++;
								}
							}
							selected.clear();
							do_paint = true;
						}
					}
				}break;
			default:
				break;
			}

			if (do_paint)
				on_paint();
			if (do_update)
				on_update();
		}
	}

	return 0;

_fail:
	system("pause");
}