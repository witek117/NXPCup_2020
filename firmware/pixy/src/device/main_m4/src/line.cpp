//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//

#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <debug.h>
#include <string.h>
#include <math.h>
#include "debug.h"
#include "calc.h"
#include "pixy_init.h"
#include "camera.h"
#include "cameravals.h"
#include "smlink.hpp"
#include "param.h"
#include "line.h"
#include "led.h"
#include "exec.h"
#include "misc.h"
#include "calc.h"
#include "simplelist.h"

#include <algorithm>  

#define ABS(x)      ((x)<0 ? -(x) : (x))
#define SIGN(x)     ((x)>=0 ? 1 : -1)

#define LINE_BUFSIZE    CAM_RES3_WIDTH/2

/********************************************************************/                    

// #define LINE_GRID_WIDTH 200
// #define LINE_GRID_HEIGHT 100

//static BarcodeLine g_barcodeLines[BARCODELINES_SIZE];
//static BarcodeCandidate g_barcodeCandidates[BARCODECANDIDATES_SIZE];    // arbitrary

/********************************************************************/

Equeue *g_equeue;

static uint16_t *g_lineBuf;

static uint16_t g_minLineWidth;
static uint16_t g_maxLineWidth;
//static uint32_t g_minLineLength2; // squared
static uint32_t g_minLineLength; // not squared
static uint16_t g_maxMergeDist; 

static uint16_t g_extractionDist;

static EqueueFields g_savedEqueue;

static uint16_t g_dist;
static uint16_t g_thresh;
static uint16_t g_hThresh;
static uint8_t g_whiteLine;
static uint8_t g_go;
static uint8_t g_repeat;
static ChirpProc g_getEdgesM0 = -1;
static ChirpProc g_setEdgeParamsM0 = -1;

LineGridNode *g_lineGrid;
static uint8_t *g_lineGridMem;
static uint8_t *g_lineSegsMem;
static LineSegIndex g_lineSegIndex;
static SimpleListNode<Line2> **g_lines;

static SimpleList<Line2> g_linesList;
static SimpleList<Point> g_nodesList;
static SimpleList<Nadir> g_nadirsList;
static SimpleList<Intersection> g_intersectionsList;
static bool g_newIntersection;static uint8_t g_primaryLineIndex;
static Point g_goalPoint;
static Point g_primaryPoint;
static bool g_primaryActive;
static uint32_t g_maxLineCompare;

static LineState g_lineState;

static uint8_t g_renderMode;

// static uint8_t g_lineFiltering;
//static uint8_t g_barcodeFiltering;

static bool g_frameFlag;
static bool g_primaryMutex;
static bool g_allMutex;

static int16_t g_defaultTurnAngle;
// static uint8_t g_delayedTurn;
// static bool g_manualVectorSelect;

bool checkGraph(int val, uint8_t suppress0=0, uint8_t suppress1=0, SimpleListNode<Intersection> *intern=NULL);

void line_shadowCallback(const char *id, const uint16_t &val);

/////////////////////////////// OUR PARAMETERS ////////////////////////////////////////////////
uint16_t detectedLinesTab[2];
uint8_t detectedLinesNumber;

const static uint8_t maxSimilarLinesToCHeck = 3;

struct LineInRow {
	enum LineType : uint8_t{
		NONE, // not detected
		EDGE = 1, // detect track edges
		PATTERN = 2, // patterns on track, 3 or 4 patternt between edges
		STOP_MAIN = 3, // diuring main competition
		STOP = 4 // smaller competition

	};/**/
	
	uint16_t center;
	LineType type;
	bool mached;
	
	LineInRow() {
		center = 0;
		type = LineType::NONE;
		mached = false;
	}
	
	LineInRow(uint16_t center_m, LineType type_m, bool mached_m) {
		center = center_m;
		type = type_m;
		mached = mached_m;
	}
	
	
	void setData(uint16_t startPixel_m, uint16_t stopPixel_m, bool valid_m) {
		center = (startPixel_m + stopPixel_m) / 2;
	}
};

LineInRow::LineType debugPrintPattern = LineInRow::LineType::EDGE;

/////////////////////////////// OUR FUNCTIONS /////////////////////////////////////////////////

class AnalyseData {
public:	
	enum LineCode{
		NONE = 1,
		LEFT = 3,
		RIGHT = 2,
		BOTH = 4
	};
	LineInRow goodDetectedLines[10];
	uint8_t goodDetectedLinesCount;
	LineInRow linesInSubRow[maxSimilarLinesToCHeck][10];
	LineInRow edgeLines[10];
	uint16_t rowIndex;
private:
	uint16_t edgeLinesDefaultWidth;
	uint16_t patternLinesDefaultWidth;

	void checkPossibleLineType(LineInRow* line, uint16_t startPixel, uint16_t stopPixel) {
		uint16_t width = stopPixel - startPixel;
		if (width < (edgeLinesDefaultWidth + 6) && width > (edgeLinesDefaultWidth - 6)) {
			line->type = LineInRow::LineType::EDGE;
		} else if (width < (patternLinesDefaultWidth + 6) && width > (patternLinesDefaultWidth - 6)) {
			line->type = LineInRow::LineType::PATTERN;
		} else {
			line->type = LineInRow::LineType::NONE;
		}
		
		line->center = (startPixel + stopPixel) / 2;
	}
	
public:
	AnalyseData(uint16_t rowIndex_m, uint8_t edgeLinesNormalWidth_m, uint16_t patternLinesDefaultWidth_m) {
		rowIndex = rowIndex_m;
		edgeLinesDefaultWidth = edgeLinesNormalWidth_m;
		patternLinesDefaultWidth = patternLinesDefaultWidth_m;
		goodDetectedLinesCount = 0;
		
		memset(linesInSubRow, 0, sizeof(LineInRow) * 10 * 3);
	}

	void analyse(uint16_t *edges, uint8_t len, uint8_t subRow) {
		LineInRow* linesInRow;
		if (subRow == 0) {
			linesInRow = linesInSubRow[0];
		} else if (subRow == 1) {
			linesInRow = linesInSubRow[1];
		} else{
			linesInRow = linesInSubRow[2];
		}
		
		memset(linesInRow, 0, sizeof(LineInRow) * 10);
		
		for (uint16_t i =0; i < len; i+=2) {
			checkPossibleLineType(&linesInRow[i/2],edges[i], edges[i+1]);
		}
		
	}
	
	LineInRow* findSimilar(LineInRow* lines, LineInRow* mainLine, uint16_t pixelsRange) {
		for(uint8_t i = 0; i < 10; i++){ 
			if (lines[i].mached == false &&
					mainLine->mached == false &&
					lines[i].type == mainLine->type &&
					lines[i].center > (mainLine->center - pixelsRange) && lines[i].center < (mainLine->center + pixelsRange) ) {
					return &lines[i];
			}
		}
		return nullptr;
	}
	
	bool check(uint8_t row, uint16_t *buf, uint32_t len) {
		uint8_t subRow = 0;
		if (row == (rowIndex - 1)) {
			subRow = 0;
		} else if (row == rowIndex) {
			subRow = 1;
		} else if (row == (rowIndex + 1)) {
			subRow = 2;
		} else {
			return false;
		}
		
		uint16_t bit0, bit1, col0, col1, lineWidth;
		uint16_t rowEdgesIndex[20];
		uint16_t indexexNumber = 0;
		for (uint16_t j=0; buf[j]<EQ_HSCAN_LINE_START && buf[j+1]<EQ_HSCAN_LINE_START && j<len; j++){
			bit0 = buf[j]&EQ_NEGATIVE;
			bit1 = buf[j+1]&EQ_NEGATIVE;
			col0 = buf[j]&~EQ_NEGATIVE;
			col1 = buf[j+1]&~EQ_NEGATIVE;
			if (bit0!=0 && bit1==0){
				lineWidth = col1 - col0;
				if (g_minLineWidth<lineWidth && lineWidth<g_maxLineWidth){
					rowEdgesIndex[indexexNumber++] = col0;
					rowEdgesIndex[indexexNumber++] = col1;
				}
			}
		}
		
		analyse(rowEdgesIndex, indexexNumber, subRow);
		goodDetectedLinesCount = 0;
		if (subRow == 2) {
			
			LineInRow* s0 = nullptr;
			LineInRow* s1 = nullptr;
			LineInRow* s2 = nullptr;
			
			for (uint8_t i = 0; i < 10; i++) {
				s0 = &linesInSubRow[0][i];
				s1 = findSimilar(linesInSubRow[1], s0, 10);
				s2 = findSimilar(linesInSubRow[2], s0, 20);
				if (s1 != nullptr && s2 != nullptr) {
					goodDetectedLines[goodDetectedLinesCount++] = LineInRow((s1->center + s2->center + s0->center) / 3, s1->type, true);
					s0->mached = true;
					s1->mached = true;
					s2->mached = true;
				} else if (s1 != nullptr) {
					goodDetectedLines[goodDetectedLinesCount++] = LineInRow((s1->center + s0->center) / 2, s1->type, true);
					s0->mached = true;
					s1->mached = true;
				} else if (s2 != nullptr) {
					goodDetectedLines[goodDetectedLinesCount++] = LineInRow((s2->center + s0->center) / 2, s2->type, true);
					s0->mached = true;
					s2->mached = true;
				}
			}
			
			for (uint8_t i = 0; i < 10; i++) {
				s0 = &linesInSubRow[1][i];
				s1 = findSimilar(linesInSubRow[2], s0, 5);
				if (s1 != nullptr) {
					goodDetectedLines[goodDetectedLinesCount++] = LineInRow((s1->center + s0->center) / 2, s1->type, true);
					s0->mached = true;
					s1->mached = true;
				}
			}
			
			for (uint8_t i =0; i < goodDetectedLinesCount; i++) {
				if (goodDetectedLines[i].type == debugPrintPattern) {
					Point p1, p2;
					p1.m_y = (row -1) / 2;
					p1.m_x = goodDetectedLines[i].center / 8;				
					g_nodesList.add(p1);					
				} 
			}
		}
		return true;
	}
};

/* AnalyseData linesData[] = {
	AnalyseData(68, 17, 51), 
	AnalyseData(62, 15, 45),
	AnalyseData(56, 14, 42),
	AnalyseData(50, 13, 39),
	AnalyseData(44, 12, 36)
};*/

struct Transition {
	enum TYPE : bool {
		W_B = 0,
		B_W = 1
	};
	uint16_t center;
	uint16_t widthToLeft;
	uint16_t widthToRight;
	TYPE type;
	
	Transition(uint16_t center_m, TYPE type_m) {
		center = center_m;
		widthToLeft = 0;
		widthToRight = 0;
		type = type_m;
	}
	
		Transition() {
		center = 0;
		widthToLeft = 0;
		widthToRight = 0;
	}
};

// static uint16_t ddd = 0;


struct Pixel {
	uint16_t x;
	uint16_t y;
	
	Pixel (uint16_t x_m, uint16_t y_m) {
		x = x_m;
		y = y_m;
	}

	
	Pixel () {
		x = 0;
		y = 0;
	}
};

struct EdgeLine {
	Pixel pixels[10];
	uint8_t detedtedLinesNumber;
	int32_t alphas[10];
	uint8_t alphasCounter;
	
	EdgeLine() {
		clear();
	}
	
	void add(Pixel pixel) {
		if (detedtedLinesNumber < 10 ) {
			pixels[detedtedLinesNumber++] = pixel;
		}
	}
	
	void add(uint16_t x_m, uint16_t y_m) {
		add(Pixel(x_m, y_m));
	}
	
	void clear() {
		detedtedLinesNumber = 0;
		alphasCounter = 0;
	}
	
	void calculateAlphas() {
		for (int8_t i = detedtedLinesNumber - 2; i >=0 ; i-- ) {
			
			
			uint16_t dy = pixels[i + 1].y -pixels[i].y;
			
			uint16_t dx = pixels[i].x - pixels[i + 1].x;
			int32_t alpha = 1024 * dy / dx;
			// addAlpha();		
			cprintf(0, "K %d %d %d %d\n", pixels[i].x, pixels[i].y, pixels[i + 1].x, pixels[i + 1].y);
			cprintf(0, "B %d %d %d %d", i, dy, dx, alpha);
		}

		
	}
	
	void addAlpha(int32_t alpha) {
		if (alphasCounter < 10) {
			alphas[alphasCounter++] = alpha;
		}
	}

};

struct Transitions {
	EdgeLine& leftEdgeLine;
	EdgeLine& rightEdgeLine;
	int rightLine;
	int leftLine;
	uint16_t edgeLineWhiteSpacing;
	uint16_t row;
	Transition transitions[20];
	Transition transitions_revert[20];
	uint16_t counter;
	
	Transitions(EdgeLine& leftEdgeLine_m, EdgeLine& rightEdgeLine_m , uint16_t row_m, uint16_t edgeLineWhiteSpacing_m) 
		: leftEdgeLine(leftEdgeLine_m), rightEdgeLine(rightEdgeLine_m) {
		counter = 0;
		edgeLineWhiteSpacing = edgeLineWhiteSpacing_m;
		row = row_m;
	}
	
	void add(uint16_t pixel, Transition::TYPE type) {
		transitions[counter++] = Transition(pixel, type);
	}
	
	void checkLine(uint8_t actualRow, uint16_t *buf, uint32_t len) {
		if (actualRow != row) return;
		
		uint16_t j;
		for (j=0; buf[j]<EQ_HSCAN_LINE_START && buf[j+1]<EQ_HSCAN_LINE_START && j<len; j++)
		{
			check(buf[j], buf[j+1], (j+2) == len, j == 0);			
		}
	}
	
	void check(uint16_t pixel0, uint16_t pixel1, bool isLast, bool isFirst) {	
		uint16_t bit0 = pixel0 & EQ_NEGATIVE;
		uint16_t bit1 = pixel1 & EQ_NEGATIVE;
		uint16_t col0 = pixel0 &~ EQ_NEGATIVE;
		uint16_t col1 = pixel1 &~ EQ_NEGATIVE;
		
		if (bit0 != 0 && bit1 == 0) { // black line
			add(col0, Transition::TYPE::W_B);
		} else if (bit0 == 0 && bit1 != 0) { // white line 
			add(col0, Transition::TYPE::B_W);
		}
		
		if (isLast) {
			add(col1, (Transition::TYPE)(!transitions[counter-1].type));
						
			findRightLine();
			findLeftLine();
			
			if (rightLine > 320 && leftLine > 320) {
				leftLine = -1;
			}
			
			if (rightLine < 320 && leftLine < 320) {
				rightLine = -1;
			}
			
			// debug
			if (rightLine >= 0) {
				rightEdgeLine.add(rightLine, row);
				g_nodesList.add(Point(rightLine / 8, row / 2));
			}

			if (leftLine >= 0) {
				leftEdgeLine.add(leftLine, row);
				g_nodesList.add(Point(leftLine / 8, row / 2));
			}
			
			counter = 0;
		}
	}
	
	void findRightLine() {
		
		uint16_t lineTransitionPixel = 0;
		for (uint8_t i = 0; i < counter; i++) {
			if (transitions[i].center < 280) {
				continue;
			}
			
			if (transitions[i].type == Transition::TYPE::W_B) {
				uint16_t whiteSpacingBefore = 640;
				
				if((i - 1) > 0){
					whiteSpacingBefore = transitions[i].center - transitions[i - 1].center;
				}
				 
				if (whiteSpacingBefore > edgeLineWhiteSpacing) {
					lineTransitionPixel = transitions[i].center;
					break;
				}
			}
		}
		
		if (lineTransitionPixel < 200) {
			rightLine = -1;
			return;
		}
		rightLine = lineTransitionPixel;
	}
	
	void findLeftLine() {
		for(uint8_t i = 0; i < counter; i++) {
			transitions_revert[i] = transitions[counter - 1 - i];
			if (transitions_revert[i].type == Transition::TYPE::B_W) {
				transitions_revert[i].type = Transition::TYPE::W_B;
			} else {
				transitions_revert[i].type = Transition::TYPE::B_W;
			}
		}
		
		uint16_t lineTransitionPixel = 0;
		for (uint8_t i = 0; i < counter; i++) {
			if (transitions_revert[i].center > 360) {
				continue;
			}
			
			if (transitions_revert[i].type == Transition::TYPE::W_B) {
				uint16_t whiteSpacingBefore =  transitions_revert[i - 1].center - transitions_revert[i].center;
				
				if (whiteSpacingBefore > edgeLineWhiteSpacing) {
					lineTransitionPixel = transitions_revert[i].center;
					break;
				}
			}
		}
		
		if (lineTransitionPixel > 440 || lineTransitionPixel < 5) {
			leftLine = -1;
			return;
		}
		
		leftLine = lineTransitionPixel;/**/
	}				
};

EdgeLine leftEdgeLine;
EdgeLine rightEdgeLine;

static Transitions transitionsToCheck[] = { 
	Transitions(leftEdgeLine, rightEdgeLine, 20, 50),
	Transitions(leftEdgeLine, rightEdgeLine, 30, 55),
	Transitions(leftEdgeLine, rightEdgeLine, 40, 60),
	Transitions(leftEdgeLine, rightEdgeLine, 50, 65),
	Transitions(leftEdgeLine, rightEdgeLine, 60, 70),
	Transitions(leftEdgeLine, rightEdgeLine, 70, 75)
};

int line_hLine(uint8_t row, uint16_t *buf, uint32_t len) {	
	if (row == 1) {
		leftEdgeLine.clear();
		rightEdgeLine.clear();
	}
		
	transitionsToCheck[0].checkLine(row, buf, len);
	transitionsToCheck[1].checkLine(row, buf, len);
	transitionsToCheck[2].checkLine(row, buf, len);
	transitionsToCheck[3].checkLine(row, buf, len);
	transitionsToCheck[4].checkLine(row, buf, len);
	transitionsToCheck[5].checkLine(row, buf, len);
	
	
	
	if (row == 90) {
		leftEdgeLine.calculateAlphas();
		
		cprintf(0, "%d %d %d %d %d",leftEdgeLine.alphas[0], leftEdgeLine.alphas[1], leftEdgeLine.alphas[2], leftEdgeLine.alphas[3], leftEdgeLine.alphas[4]);
	}
	
	
	uint8_t line = 4;
	
	
	if (transitionsToCheck[line].rightLine >= 0 && transitionsToCheck[line].leftLine >= 0) {
		detectedLinesTab[0] = transitionsToCheck[line].leftLine;
		detectedLinesTab[1] = transitionsToCheck[line].rightLine;
		detectedLinesNumber = (uint8_t)AnalyseData::LineCode::BOTH;
	} else if (transitionsToCheck[line].rightLine >= 0) {
		detectedLinesTab[0] = transitionsToCheck[line].rightLine;
		detectedLinesNumber = (uint8_t)AnalyseData::LineCode::RIGHT;
	} else if (transitionsToCheck[line].leftLine >= 0) {
		detectedLinesTab[0] = transitionsToCheck[line].leftLine;
		detectedLinesNumber = (uint8_t)AnalyseData::LineCode::LEFT;
	} else {
		detectedLinesNumber = (uint8_t)AnalyseData::LineCode::NONE;
	}

	return 0;
}



/////////////////////////////////////// OTHER FUNCTION /////////////////////////////////////////////


static const ProcModule g_module[] =
{
    END
};
 
void line_shadowCallback(const char *id, const void *val)
{
    int responseInt;
    bool callM0 = false;
//    uint16_t leading, trailing;
    
    if (strcmp(id, "Edge distance")==0)
    {
        g_dist = *(uint16_t *)val;
        callM0 = true;
    }
    else if (strcmp(id, "Edge threshold")==0)
    {
        g_thresh = *(uint16_t *)val;
        g_hThresh = g_thresh*LINE_HTHRESH_RATIO;    
        callM0 = true;
    }
    else if (strcmp(id, "Minimum line width")==0)
        g_minLineWidth = *(uint16_t *)val;
    else if (strcmp(id, "Maximum line width")==0)
        g_maxLineWidth = *(uint16_t *)val;
    else if (strcmp(id, "Line extraction distance")==0)
        g_extractionDist = *(uint16_t *)val;
    else if (strcmp(id, "Maximum merge distance")==0)
        g_maxMergeDist = *(uint16_t *)val;
    else if (strcmp(id, "Minimum line length")==0)
    {
        g_minLineLength = *(uint16_t *)val;
        //g_minLineLength2 = g_minLineLength*g_minLineLength; // squared
    }
    // else if (strcmp(id, "Maximum line compare")==0)
    //     g_maxLineCompare = *(uint32_t *)val; 
    // else if (strcmp(id, "White line")==0)
    //     g_whiteLine = *(uint8_t *)val;
    // else if (strcmp(id, "Manual vector select")==0)
    //    g_manualVectorSelect = *(uint8_t *)val;
    // else if (strcmp(id, "Line filtering")==0)
    //     g_lineFiltering = *(uint8_t *)val;
    //else if (strcmp(id, "Intersection filtering")==0)
    //{
//        uint8_t v;
       // v = *(uint8_t *)val;
        //leading = v*LINE_FILTERING_MULTIPLIER;
//        trailing = (leading+1)>>1;
        //g_primaryIntersection.setTiming(leading, trailing); 
    //}
    //else if (strcmp(id, "Barcode filtering")==0)
    //    g_barcodeFiltering = *(uint8_t *)val;
    // else if (strcmp(id, "Delayed turn")==0)
    //     g_delayedTurn = *(uint8_t *)val;
    else if (strcmp(id, "Go")==0)
        g_go = *(uint8_t *)val;
    else if (strcmp(id, "Repeat")==0)
        g_repeat = *(uint8_t *)val;

    if (callM0)
        g_chirpM0->callSync(g_setEdgeParamsM0, UINT16(g_dist), UINT16(g_thresh), UINT16(g_hThresh), END_OUT_ARGS, &responseInt, END_IN_ARGS);
}



int line_loadParams(int8_t progIndex)
{    
	cprintf(0, "load params");
	
    int i, responseInt=-1;
    char id[32], desc[128];
//    uint16_t leading, trailing;
    
    // add params
	
    if (progIndex>=0)
    {
        prm_add("Edge distance", PROG_FLAGS(progIndex) | PRM_FLAG_SLIDER, PRM_PRIORITY_4,
            "@c Expert @m 1 @M 15 Sets the distance between pixels when computing edges (default " STRINGIFY(LINE_EDGE_DIST_DEFAULT) ")", UINT16(LINE_EDGE_DIST_DEFAULT), END);
        prm_setShadowCallback("Edge distance", (ShadowCallback)line_shadowCallback);

        prm_add("Edge threshold", PROG_FLAGS(progIndex) | PRM_FLAG_SLIDER, PRM_PRIORITY_5,
            "@c Tuning @m 1 @M 150 Sets edge detection threshold (default " STRINGIFY(LINE_EDGE_THRESH_DEFAULT) ")", UINT16(LINE_EDGE_THRESH_DEFAULT), END);
        prm_setShadowCallback("Edge threshold", (ShadowCallback)line_shadowCallback);

        prm_add("Minimum line width", PROG_FLAGS(progIndex) | PRM_FLAG_SLIDER, PRM_PRIORITY_5, 
            "@c Tuning @m 0 @M 100 Sets minimum detected line width " STRINGIFY(LINE_MIN_WIDTH) ")", UINT16(LINE_MIN_WIDTH), END);
        prm_setShadowCallback("Minimum line width", (ShadowCallback)line_shadowCallback);

        prm_add("Maximum line width", PROG_FLAGS(progIndex) | PRM_FLAG_SLIDER, PRM_PRIORITY_5, 
            "@c Tuning @m 1 @M 250 Sets maximum detected line width " STRINGIFY(LINE_MAX_WIDTH) ")", UINT16(LINE_MAX_WIDTH), END);
        prm_setShadowCallback("Maximum line width", (ShadowCallback)line_shadowCallback);    

        prm_add("Line extraction distance", PROG_FLAGS(progIndex) | PRM_FLAG_SLIDER, PRM_PRIORITY_4,
            "@c Expert @m 1 @M 25 Sets the distance to search when extracting lines (default " STRINGIFY(LINE_EXTRACTION_DIST_DEFAULT) ")", UINT16(LINE_EXTRACTION_DIST_DEFAULT), END);
        prm_setShadowCallback("Line extraction distance", (ShadowCallback)line_shadowCallback);

        prm_add("Maximum merge distance", PROG_FLAGS(progIndex) | PRM_FLAG_SLIDER, PRM_PRIORITY_4, 
            "@c Expert @m 0 @M 25 Sets the search distance for merging lines (default " STRINGIFY(LINE_MAX_MERGE_DIST) ")", UINT16(LINE_MAX_MERGE_DIST), END);
        prm_setShadowCallback("Maximum merge distance", (ShadowCallback)line_shadowCallback);

        prm_add("Minimum line length", PROG_FLAGS(progIndex) | PRM_FLAG_SLIDER, PRM_PRIORITY_4, 
            "@c Expert @m 1 @M 50 Sets the minimum line length (default " STRINGIFY(LINE_MIN_LENGTH) ")", UINT16(LINE_MIN_LENGTH), END);
        prm_setShadowCallback("Minimum line length", (ShadowCallback)line_shadowCallback);

        prm_add("Maximum line compare", PROG_FLAGS(progIndex) | PRM_FLAG_SLIDER, PRM_PRIORITY_4, 
            "@c Expert @m 1 @M 10000 Sets the maximum distance between lines for them to be considered the same line between frames (default " STRINGIFY(LINE_MAX_COMPARE) ")", UINT32(LINE_MAX_COMPARE), END);
        prm_setShadowCallback("Maximum line compare", (ShadowCallback)line_shadowCallback);


       
         prm_add("Go", PROG_FLAGS(progIndex) | PRM_FLAG_CHECKBOX  
            | PRM_FLAG_INTERNAL, 
            PRM_PRIORITY_4,
            "@c Expert Debug flag. (default false)", UINT8(0), END);
        prm_setShadowCallback("Go", (ShadowCallback)line_shadowCallback);
/**/
        prm_add("Repeat", PROG_FLAGS(progIndex) | PRM_FLAG_CHECKBOX 
            | PRM_FLAG_INTERNAL, 
            PRM_PRIORITY_4,
            "@c Expert Debug flag. (default false)", UINT8(0), END);
        prm_setShadowCallback("Repeat", (ShadowCallback)line_shadowCallback);
    
        for (i=0; i<16; i++)
        {
            sprintf(id, "Barcode label %d", i);
            sprintf(desc, "@c Barcode_Labels Sets the label for barcodes that match barcode pattern %d.", i);
            prm_add(id, PROG_FLAGS(progIndex), PRM_PRIORITY_3-i, desc, STRING(""), END);
        }
				
				sprintf(id, "Barcode label kupa");
        sprintf(desc, "@c Barcode_Labelk Sets the label for barcodes that match barcode pattern kupa.");
        prm_add(id, PROG_FLAGS(progIndex), PRM_PRIORITY_3-i, desc, STRING(""), END);
    }
    
    // load params
   prm_get("Edge distance", &g_dist, END);    
    prm_get("Edge threshold", &g_thresh, END);    
    g_hThresh = g_thresh*LINE_HTHRESH_RATIO;
    prm_get("Minimum line width", &g_minLineWidth, END);
    prm_get("Maximum line width", &g_maxLineWidth, END);
    prm_get("Line extraction distance", &g_extractionDist, END);
    prm_get("Maximum merge distance", &g_maxMergeDist, END);
    prm_get("Minimum line length", &g_minLineLength, END);
    //g_minLineLength2 = g_minLineLength*g_minLineLength; // square it 
    prm_get("Maximum line compare", &g_maxLineCompare, END);
    // prm_get("White line", &g_whiteLine, END);
    // prm_get("Intersection filtering", &g_lineFiltering, END);
//    leading = g_lineFiltering*LINE_FILTERING_MULTIPLIER;
  //  trailing = (leading+1)>>1;
    //g_primaryIntersection.setTiming(leading, trailing); 
    // prm_get("Line filtering", &g_lineFiltering, END);
    // prm_get("Barcode filtering", &g_barcodeFiltering, END);
    // prm_get("Default turn angle", &g_defaultTurnAngle, END);
    // prm_get("Delayed turn", &g_delayedTurn, END);
    // prm_get("Manual vector select", &g_manualVectorSelect, END);
    prm_get("Go", &g_go, END);    
    prm_get("Repeat", &g_repeat, END);    
    
    g_chirpM0->callSync(g_setEdgeParamsM0, UINT16(g_dist), UINT16(g_thresh), UINT16(g_hThresh), END_OUT_ARGS, &responseInt, END_IN_ARGS);
    
    return responseInt;
}

int line_init(Chirp *chirp)
{        
    chirp->registerModule(g_module);    

    g_getEdgesM0 = g_chirpM0->getProc("getEdges", NULL);
    g_setEdgeParamsM0 = g_chirpM0->getProc("setEdgeParams", NULL);
    
    if (g_getEdgesM0<0 || g_setEdgeParamsM0<0)
        return -1;

    return 0;
}

int line_open(int8_t progIndex)
{
    g_linesList.clear();
    g_nodesList.clear();
    g_nadirsList.clear();
    g_intersectionsList.clear();

    g_lineGridMem = (uint8_t *)malloc(LINE_GRID_WIDTH*LINE_GRID_HEIGHT*sizeof(LineGridNode)+CAM_PREBUF_LEN+8); // +8 for extra memory at the end because little overruns sometimes happen
    g_lineGrid = (LineGridNode *)(g_lineGridMem+CAM_PREBUF_LEN);
    
    g_lines = (SimpleListNode<Line2> **)malloc(LINE_MAX_LINES*sizeof(SimpleListNode<Line2> *));
    g_lineSegsMem = (uint8_t *)malloc(LINE_MAX_SEGMENTS*sizeof(LineSeg)+CAM_PREBUF_LEN);
    
    g_lineBuf = (uint16_t *)malloc(LINE_BUFSIZE*sizeof(uint16_t)); 
    g_equeue = new (std::nothrow) Equeue;
    
    g_lineState = LINE_STATE_ACQUIRING;
    g_primaryActive = false;
    g_newIntersection = false;
    // g_delayedTurn = false;
    g_defaultTurnAngle = 0;

    // g_manualVectorSelect = false;
    
    g_renderMode = LINE_RM_ALL_FEATURES;
    
    if (g_equeue==NULL || g_lineBuf==NULL || g_lineGridMem==NULL || g_lineSegsMem==NULL || g_lines==NULL )
    {
        cprintf(0, "Line memory error\n");
        line_close();
        return -1;
    }
    
    g_repeat = 0;
    
    g_frameFlag = false;
    
    return line_loadParams(progIndex);
}

void line_close()
{
    if (g_equeue)
        delete g_equeue;
    if (g_lineBuf)
        free(g_lineBuf);
    if (g_lineGridMem)
        free(g_lineGridMem);
    if (g_lineSegsMem)
        free(g_lineSegsMem);
    if (g_lines)
        free(g_lines);
    g_linesList.clear();
    g_nodesList.clear();
    g_nadirsList.clear();
    g_intersectionsList.clear();
}

int32_t line_getEdges()
{
    int32_t responseInt = -1;
    // forward call to M0, get edges
    g_chirpM0->callSync(g_getEdgesM0, UINT32(SRAM1_LOC+CAM_PREBUF_LEN), END_OUT_ARGS, &responseInt, END_IN_ARGS);
        
    return responseInt;
}

int line_vLine(uint8_t row, uint8_t *vstate, uint16_t *buf, uint32_t len)
{
    uint16_t i, index, bit0, col0, lineWidth;

	// 
		// if (row != 200) return 0;
    
	if (row == 50) {
		//cprintf(0, "row %d %d\n", row, len);
		
		for (i=0; buf[i]<EQ_HSCAN_LINE_START && i<len; i++)
		{
				bit0 = buf[i]&EQ_NEGATIVE;
				col0 = (buf[i]&~EQ_NEGATIVE)>>2;
				if (bit0!=0) // neg
						vstate[col0] = row+1;
				else // bit0==0, pos
				{
						if (vstate[col0]!=0)
						{
								lineWidth = (row - (vstate[col0]-1))<<2; // multiply by 4 because vertical is subsampled by 4
								if (g_minLineWidth<lineWidth && lineWidth<g_maxLineWidth && col0<LINE_VSIZE)
								{
										index = LINE_GRID_INDEX(col0>>1, (row - (lineWidth>>3))>>1);
										if (index<LINE_GRID_WIDTH*LINE_GRID_HEIGHT+8)
												g_lineGrid[index] |= LINE_NODE_FLAG_VLINE;
										else
												cprintf(0, "high index\n");
								}
								vstate[col0] = 0;
						}
				}
		}
	}/**/
    
    return 0;
}

int line_sendLineGrid(uint8_t renderFlags)
{
    uint32_t len;
    uint8_t *gridData = (uint8_t *)g_lineGridMem + CAM_PREBUF_LEN - CAM_FRAME_HEADER_LEN;
    
    // fill buffer contents manually for return data 
    len = Chirp::serialize(g_chirpUsb, (uint8_t *)gridData, LINE_GRID_WIDTH*LINE_GRID_HEIGHT*sizeof(LineGridNode), HTYPE(FOURCC('L','I','N','G')), HINT8(renderFlags), UINT16(LINE_GRID_WIDTH), UINT16(LINE_GRID_HEIGHT), UINTS16_NO_COPY(LINE_GRID_WIDTH*LINE_GRID_HEIGHT), END);
    if (len!=CAM_FRAME_HEADER_LEN)
        return -1;
    
    g_chirpUsb->useBuffer((uint8_t *)gridData, CAM_FRAME_HEADER_LEN+LINE_GRID_WIDTH*LINE_GRID_HEIGHT*sizeof(LineGridNode)); 
    cprintf(0, "%s\n", gridData);
    return 0;
}

bool validIntersection(SimpleListNode<Intersection> *intern)
{
    SimpleListNode<Intersection> *i;
    
    if (intern==NULL)
        return true; // valid

    for (i=g_intersectionsList.m_first; i!=NULL; i=i->m_next)
    {
        if (i==intern)
            return true;
    }
    return false;
}

bool validLine(SimpleListNode<Line2> *linen)
{
    SimpleListNode<Line2> *i;
    
    if (linen==NULL)
        return true; // valid

    for (i=g_linesList.m_first; i!=NULL; i=i->m_next)
    {
        if (i==linen)
            return true;
    }
    return false;
}


#define CHECK(cond, index)   if(!(cond) && suppress0!=index && suppress1!=index) {check=index; goto end;} 

bool checkGraph(int val, uint8_t suppress0, uint8_t suppress1, SimpleListNode<Intersection> *intern)
{
    SimpleListNode<Line2> *i;
    SimpleListNode<Intersection> *j;
    uint8_t k, n, h, check=0;    
    
    if (!(g_debug&LINE_DEBUG_GRAPH_CHECK))
        return true;
    
    if (intern)
        goto intersection;
    
    // go through line list, make sure all intersection pointers are valid 
    // by finding them in the list, make sure the line is in the intersection's list
    for (i=g_linesList.m_first; i!=NULL; i=i->m_next)
    {
        CHECK(validIntersection(i->m_object.m_i0), 1);
        CHECK(validIntersection(i->m_object.m_i1), 2);
        if (i->m_object.m_i0)
        {
            for (k=n=0; k<i->m_object.m_i0->m_object.m_n; k++)
            {
                if (i->m_object.m_i0->m_object.m_lines[k]==i)
                    n++;
            }
            CHECK(n==1, 3);
        }
        if (i->m_object.m_i1)
        {
            for (k=n=0; k<i->m_object.m_i1->m_object.m_n; k++)
            {
                if (i->m_object.m_i1->m_object.m_lines[k]==i)
                    n++;
            }
            CHECK(n==1, 4);
        }        
    }
    
    intersection:
    // go through intersection list, make sure each intersection's lines are valid
    // make sure each line points back to the intersection
    for (j=g_intersectionsList.m_first; j!=NULL; j=j->m_next)
    {
        if (intern)
            j = intern;
        for (k=n=0; k<j->m_object.m_n; k++)
        {
            CHECK(validLine(j->m_object.m_lines[k]), 5);
            CHECK(j->m_object.m_lines[k]->m_object.m_i0 != j->m_object.m_lines[k]->m_object.m_i1, 6);
            CHECK(j->m_object.m_lines[k]->m_object.m_i0==j || j->m_object.m_lines[k]->m_object.m_i1==j, 7);
            if (j->m_object.m_lines[k]->m_object.m_i0==j)
                CHECK(j->m_object.m_p.equals(j->m_object.m_lines[k]->m_object.m_p0), 8);
            if (j->m_object.m_lines[k]->m_object.m_i1==j)
                CHECK(j->m_object.m_p.equals(j->m_object.m_lines[k]->m_object.m_p1), 9);            
            for (h=k+1; h<j->m_object.m_n; h++)
            {
                if (j->m_object.m_lines[k]==j->m_object.m_lines[h])
                    n++;
            }
            CHECK(n==0, 10);
        }
        if (intern)
            break;
    }
    
    end:
    if (check!=0)
    {
        cprintf(0, "fail %d %d\n", check, val);
        return false;
    }
        
    return true;
}

int sendLineSegments(uint8_t renderFlags)
{
    uint32_t len;
    uint8_t *lineData = (uint8_t *)g_lineSegsMem + CAM_PREBUF_LEN - CAM_FRAME_HEADER_LEN;
        // g_lineSegs[0].m_p0.m_x = 0;
        // g_lineSegs[0].m_p0.m_y = 0;
        // g_lineSegs[0].m_p1.m_x = 10;
        // g_lineSegs[0].m_p1.m_y = 10;
    
    // fill buffer contents manually for return data 
    len = Chirp::serialize(g_chirpUsb, (uint8_t *)lineData, LINE_MAX_SEGMENTS*sizeof(LineSeg), HTYPE(FOURCC('L','I','S','G')), HINT8(renderFlags), UINT16(LINE_GRID_WIDTH), UINT16(LINE_GRID_HEIGHT), UINTS8_NO_COPY(g_lineSegIndex*sizeof(LineSeg)), END);
    if (len!=CAM_FRAME_HEADER_LEN)
        return -1;
    
    g_chirpUsb->useBuffer((uint8_t *)lineData, CAM_FRAME_HEADER_LEN+g_lineSegIndex*sizeof(LineSeg)); 
    
    return 0;
}

void sendPoints(const SimpleList<Point> &points, uint8_t renderFlags, const char *desc)
{
    SimpleListNode<Point> *i;

    CRP_SEND_XDATA(g_chirpUsb, HTYPE(FOURCC('N','A','D','F')), INT8(RENDER_FLAG_START), STRING(desc), INT16(LINE_GRID_WIDTH), INT16(LINE_GRID_HEIGHT), END);
    
    for (i=points.m_first; i!=NULL; i=i->m_next)        
        CRP_SEND_XDATA(g_chirpUsb, HTYPE(FOURCC('N','A','D','S')), INTS8(2, &i->m_object), END);    
    
    CRP_SEND_XDATA(g_chirpUsb, HTYPE(FOURCC('N','A','D','F')), INT8(renderFlags), STRING(desc), INT16(LINE_GRID_WIDTH), INT16(LINE_GRID_HEIGHT), END);
}

void sendPrimaryFeatures(uint8_t renderFlags) {
    uint8_t n=0;
    Line2 *primary=NULL;
    
    if (primary)
    {
        if (g_goalPoint.equals(primary->m_p0))
            CRP_SEND_XDATA(g_chirpUsb, HTYPE(FOURCC('P','V','I','0')), INT8(renderFlags), INT16(LINE_GRID_WIDTH), INT16(LINE_GRID_HEIGHT), 
                INT8(primary->m_p1.m_x), INT8(primary->m_p1.m_y), INT8(primary->m_p0.m_x), INT8(primary->m_p0.m_y), INT8(n), END);
        else
            CRP_SEND_XDATA(g_chirpUsb, HTYPE(FOURCC('P','V','I','0')), INT8(renderFlags), INT16(LINE_GRID_WIDTH), INT16(LINE_GRID_HEIGHT), 
                INT8(primary->m_p0.m_x), INT8(primary->m_p0.m_y), INT8(primary->m_p1.m_x), INT8(primary->m_p1.m_y), INT8(n), END);
    }
    else // send null message so it always shows up as a layer
        CRP_SEND_XDATA(g_chirpUsb, HTYPE(FOURCC('P','V','I','0')), INT8(renderFlags), INT16(LINE_GRID_WIDTH), INT16(LINE_GRID_HEIGHT), 
            INT8(0), INT8(0), INT8(0), INT8(0), INT8(0), END);
}

uint32_t dist2_4(const Point16 &p0, const Point16 &p1)
{
        int32_t diffx, diffy;
        
        diffx = p1.m_x - p0.m_x;
        diffy = (p1.m_y - p0.m_y)*4;
        
        return diffx*diffx + diffy*diffy;    
}


int line_processMain()
{
    static uint32_t n = 0;
    uint32_t i;
    uint32_t len, tlen;
    bool eof, error;
    int8_t row;
    uint8_t vstate[LINE_VSIZE];
    uint32_t timer;
    SimpleList<uint32_t> timers;  

    // send frame and data over USB 
    if (g_renderMode!=LINE_RM_MINIMAL)
        cam_sendFrame(g_chirpUsb, CAM_RES3_WIDTH, CAM_RES3_HEIGHT, 
            RENDER_FLAG_BLEND, FOURCC('4', '0', '1', '4'));

    // indicate start of edge data
    if (g_debug&LINE_DEBUG_LAYERS)
        CRP_SEND_XDATA(g_chirpUsb, HTYPE(FOURCC('E','D','G','F')), HINT8(RENDER_FLAG_START), HINT16(CAM_RES3_WIDTH), HINT16(CAM_RES3_HEIGHT), END);
    
    // initialize variables
    //g_lineIndex = 1; // set to 1 because 0 means empty...
    g_lineSegIndex = 0;
    memset(vstate, 0, LINE_VSIZE);
    memset(g_lineGrid, 0, LINE_GRID_WIDTH*LINE_GRID_HEIGHT*sizeof(LineGridNode));
    
		cprintf(0, "g_debug %d\n", g_debug);		
    
    if (g_debug==LINE_DEBUG_BENCHMARK)
        timers.clear(); // need to clear timers because of repeat flag, leads to memory leak
    
    if (g_debug&LINE_DEBUG_TRACKING)
        cprintf(0, "Frame %d ______\n", n++);
    
    checkGraph(__LINE__);

    if (g_repeat)
        *g_equeue->m_fields = g_savedEqueue;
    else
        g_savedEqueue = *g_equeue->m_fields;
		
    g_nodesList.clear();
    
		setTimer(&timer);
    for (i=0, row=-1, tlen=0; true; i++)
    {
        while((len=g_equeue->readLine(g_lineBuf, LINE_BUFSIZE, &eof, &error))==0)
        {    
            if (getTimer(timer)>100000)
            {
                error = true;
                cprintf(0, "line hang\n");
                goto outside;
            }
        }
        tlen += len;
        if (g_lineBuf[0]==EQ_HSCAN_LINE_START)
        {
            row++;
            line_hLine(row, g_lineBuf+1, len-1);
        }
        else if (g_lineBuf[0]==EQ_VSCAN_LINE_START) {
					
            // line_vLine(row, vstate, g_lineBuf+1, len-1);
				}
        if (g_debug&LINE_DEBUG_LAYERS)
            CRP_SEND_XDATA(g_chirpUsb, HTYPE(FOURCC('E','D','G','S')), UINTS16(len, g_lineBuf), END);
        if (eof || error)
            break;
    }
    
    outside:
    // indicate end of edge data
    if (g_debug&LINE_DEBUG_LAYERS)
        CRP_SEND_XDATA(g_chirpUsb, HTYPE(FOURCC('E','D','G','F')), HINT8(0), HINT16(CAM_RES3_WIDTH), HINT16(CAM_RES3_HEIGHT), END);

    if (g_debug==LINE_DEBUG_BENCHMARK)
        timers.add(getTimer(timer));

    if (g_debug==LINE_DEBUG_BENCHMARK)
        setTimer(&timer);

    if (g_debug==LINE_DEBUG_BENCHMARK)
        timers.add(getTimer(timer));
    
    g_linesList.clear();

    g_nadirsList.clear();
    g_intersectionsList.clear();
    
    if (error) // deal with error after we call clustercodes otherwise there's a memory leak
    {
        cprintf(0, "error\n");
        g_equeue->flush();
        
        return -1;
    }
        
    g_allMutex = true;
    g_primaryMutex = true;
    g_primaryMutex = false;
    
    if (g_debug&LINE_DEBUG_LAYERS)
        line_sendLineGrid(0);
    
    if (g_debug==LINE_DEBUG_BENCHMARK)
        setTimer(&timer);
		
    if (g_debug==LINE_DEBUG_BENCHMARK)
        timers.add(getTimer(timer));
		
    // sendLineSegments(0);
    sendPoints(g_nodesList, 0, "nodes");
		
    g_allMutex = false;
    // render whatever we've sent
    exec_sendEvent(g_chirpUsb, EVT_RENDER_FLUSH);
    
    g_frameFlag = true;
    
    return 0;
}

int line_process()
{
    if (!g_repeat)
    {
        if (g_renderMode!=LINE_RM_MINIMAL)
        {
            SM_OBJECT->currentLine = 0;
            SM_OBJECT->stream = 1; // set streaming

            // wait for frame
            while(SM_OBJECT->currentLine < CAM_RES3_HEIGHT-1)
            {
                // After grab starts, reset streaming so we don't accidentally grab the next frame.
                // If we accidentally grab the next frame, we get overruns in the equeue, etc. 
                // This only happens when we're running PixyMon (streaming over USB) and we're 
                // getting hammered with serial interrupts for communication.  This would sometimes
                // lead to this loop erroring -- we miss currentline and it wraps around.
                if (SM_OBJECT->currentLine>1)
                    SM_OBJECT->stream = 0;
            }
        }
        else
            SM_OBJECT->stream = 1; // set streaming                    
    }
        
    return line_processMain();
}

int line_setRenderMode(uint8_t mode)
{
    g_renderMode = mode;
    if (mode>LINE_RM_ALL_FEATURES)
        return -1;
    return 0;
}

uint8_t line_getRenderMode()
{
    return g_renderMode;
}


int line_getPrimaryFrame(uint8_t typeMap, uint8_t *buf, uint16_t len)
{
    uint16_t length = 0;
    
    // deal with g_frameFlag -- return error when it's false, indicating no new data
    if (!g_frameFlag || g_primaryMutex)
        return -1; // no new data, or busy
    g_frameFlag = false;
    
    // only return primary line if we're tracking and primary line is in active (valid) state
    if (g_lineState==LINE_STATE_TRACKING && g_primaryActive)
    {
        // we assume that we can fit all 3 features in a single packet (255 bytes)
        if (typeMap&LINE_FR_VECTOR_LINES)
        {
            // line information is always present
            FrameLine *line;
            *(uint8_t *)(buf + length) = LINE_FR_VECTOR_LINES;
            *(uint8_t *)(buf + length + 1) = sizeof(FrameLine);
            line = (FrameLine *)(buf + length + 2);
            line->m_x0 = g_primaryPoint.m_x;
            line->m_y0 = g_primaryPoint.m_y;
            line->m_x1 = g_goalPoint.m_x;
            line->m_y1 = g_goalPoint.m_y;
            line->m_index = g_primaryLineIndex; 
            line->m_flags = 0;
            if (g_intersectionsList.m_size)
                line->m_flags |= LINE_FR_FLAG_INTERSECTION;
            length += sizeof(FrameLine) + 2;
        }
        // Intersection information, only present when intersection appears
        if ((typeMap&LINE_FR_INTERSECTION) && g_newIntersection)
        {
            *(uint8_t *)(buf + length) = LINE_FR_INTERSECTION;
            *(uint8_t *)(buf + length + 1) = sizeof(FrameIntersection);
            //memcpy(buf+length+2, &g_primaryIntersection.m_object, sizeof(FrameIntersection));
            length += sizeof(FrameIntersection) + 2;
            
            g_newIntersection = false;
        }
    }
    return length;
}

int line_getPrimaryFrame2(uint8_t typeMap, uint8_t *buf, uint16_t len)
{
    uint16_t length = 0;
    
    // deal with g_frameFlag -- return error when it's false, indicating no new data
    if (!g_frameFlag || g_primaryMutex)
        return -1; // no new data, or busy
    g_frameFlag = false;
    
    // only return primary line if we're tracking and primary line is in active (valid) state
    if (g_lineState==LINE_STATE_TRACKING && g_primaryActive)
    {
        // we assume that we can fit all 3 features in a single packet (255 bytes)
        if (typeMap&LINE_FR_VECTOR_LINES)
        {
            // line information is always present
            FrameLine *line;
            *(uint8_t *)(buf + length) = LINE_FR_VECTOR_LINES;
            *(uint8_t *)(buf + length + 1) = sizeof(FrameLine);
            line = (FrameLine *)(buf + length + 2);
            line->m_x0 = 0;
            line->m_y0 = 0;
            line->m_x1 = 100;
            line->m_y1 = 100;
            line->m_index = g_primaryLineIndex; 
            line->m_flags = 0;
            if (g_intersectionsList.m_size)
                line->m_flags |= LINE_FR_FLAG_INTERSECTION;
            length += sizeof(FrameLine) + 2;
        }
    }
    return length;
}

int line_setMode(int8_t modeMap)
{
    // g_delayedTurn = (modeMap & LINE_MODEMAP_TURN_DELAYED) ? true : false;
    g_whiteLine = (modeMap & LINE_MODEMAP_WHITE_LINE) ? true : false;
    // g_manualVectorSelect = (modeMap & LINE_MODEMAP_MANUAL_SELECT_VECTOR) ? true : false;
    return 0;
}

/* int line_legoLineData(uint8_t *buf, uint32_t buflen)
{
//    Line2 *primary;
    uint32_t x;
    static uint8_t lastData[4];

    // override these because LEGO mode doesn't support 
    //sg_delayedTurn = false;
    g_manualVectorSelect = false;
    
    if (g_allMutex || !g_frameFlag) 
    {
        memcpy(buf, lastData, 4); // use last data
        return 4; // busy or no new eata
    }
    
    g_frameFlag = false;
    
    buf[2] = (uint8_t)-1;
    
    // only return primary line if we're tracking and primary line is in active (valid) state
    if (g_lineState==LINE_STATE_TRACKING && g_primaryActive)
    {
        x = (g_goalPoint.m_x *128)/78; // scale to 0 to 128
        buf[0] = x;
        if (g_goalPoint.m_y > g_primaryPoint.m_y)
            buf[3] = 1;
        else
            buf[3] = 0;
    }
    else
    {
        buf[0] = (uint8_t)-1;
        buf[3] = 0;
    }   
    
    if (g_newIntersection)
        g_newIntersection = false;
    
    memcpy(lastData, buf, 4);   
    return 4;
}
*/
