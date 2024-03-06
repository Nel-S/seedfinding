// Imports settings
#include "settings.h"

// Assumes a copy of, at bare minimum, Cubiomes's noise.c, noise.h, and rng.h in a neighboring folder called "cubiomes". (See GitHub link in main file.)
#include "../../../../cubiomes/noise.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>

const double TWO_PI = 6.2831853071795864769252866;
// Amplitudes of each climate
const double AMPLITUDES[] = {5./4, 10./9, 3./2, 25./18, 5./4, 5./4};
const double MAX_PERLIN_VALUE = 1.0363538112118025;

// Struct for interpreting the parameters sent to each thread's *checkSeed() function
typedef struct {
	uint64_t id;
} ThreadData;

#if CHECK_DISTANCES
// Calculates the corresponding MIN_DESIRED_RING if only a distance is provided by the user
// (Should be 0 if MIN_RADIAL_DISTANCE <= 512., but 0 is not supported) 
// TODO: Redo in terms of fitness lookup table using COORDS[][4]
const uint8_t MIN_DESIRED_RING = (MIN_RADIAL_DISTANCE <= 1024.) ? 1 :
								 (MIN_RADIAL_DISTANCE <= 1536.) ? 2 :
								 (MIN_RADIAL_DISTANCE <= 2048.) ? 3 : 4;
#endif

/* Index at which each ring begins. Given the COORDS[] coordinates are sorted by ascending distance, this is also the index of the smallest distance in each ring, while this minus one
   is the index of the largest distance in the previous ring.*/
const uint8_t RING_STARTING_INDEX = (!MIN_DESIRED_RING     ) ? (int)(0*TWO_PI) + 1 :
									( MIN_DESIRED_RING == 1) ? (int)(0*TWO_PI) + (int)(1*TWO_PI) + 2 :
									( MIN_DESIRED_RING == 2) ? (int)(0*TWO_PI) + (int)(1*TWO_PI) + (int)(2*TWO_PI) + 3 :
									( MIN_DESIRED_RING == 3) ? (int)(0*TWO_PI) + (int)(1*TWO_PI) + (int)(2*TWO_PI) + (int)(3*TWO_PI) + 4 :
															   (int)(0*TWO_PI) + (int)(1*TWO_PI) + (int)(2*TWO_PI) + (int)(3*TWO_PI) + (int)(4*TWO_PI) + 5;
					                  						   
#if CHECK_DISTANCES
const double MIN_DIST = (MIN_RADIAL_DISTANCE < MIN_AXIAL_DISTANCE ? MIN_RADIAL_DISTANCE : MIN_AXIAL_DISTANCE)/25;
const double CONT_FITNESS_SQRT = MOST_POSITIVE_CONT < -0.11 ? 10000*MOST_POSITIVE_CONT + 1100 : 0;
const double NON_RING_FITNESS = MIN_DIST * MIN_DIST * MIN_DIST * MIN_DIST + CONT_FITNESS_SQRT * CONT_FITNESS_SQRT;
#endif

/* Lookup table containing
 - each x-coordinate checked by the algorithm's first stage, right-bitshifted by two (x >> 2),
 - the bitshifted x-coordinate times 337/331,
 - each z-coordinate checked in the first stage, right-bitshifted by two (z >> 2),
 - the bitshifted z-coordinate times 337/331, and
 - the corresponding calculated distance fitness ((x^2 + z^2)^2 / 25^4),
 sorted by ascending fitness. (Each x/z-coordinate is found via the formula ((int)(512*ring*sin(i/ring)), (int)(512*ring*cos(i/ring))) for i in [0, floor(ring*2pi)].)*/
const double COORDS[][5] = {
	// Zeroth ring coordinates (~0 blocks away)
	{0, 0, 0, 0, 0},
	// First ring coordinates (~512 blocks away)
	{107, 107*337./331, 69, 69*337./331, 174491}, {-123, -123*337./331, 36, 36*337./331, 174556}, {18, 18*337./331, -127, -127*337./331, 174683},
	{-97, -97*337./331, -84, -84*337./331, 174824}, {-36, -36*337./331, 122, 122*337./331, 175098}, {116, 116*337./331, -54, -54*337./331, 175184},
	{0, 0, 128, 128*337./331, 175921},
	// Second ring coordinates (~1024 blocks away)
	{-250, -250*337./331, -54, -54*337./331, 2802142}, {-246, -246*337./331, 72, 72*337./331, 2803406}, {122, 122*337./331, 224, 224*337./331, 2803636},
	{-194, -194*337./331, -168, -168*337./331, 2804349}, {-90, -90*337./331, -240, -240*337./331, 2804392}, {36, 36*337./331, -254, -254*337./331, 2805785},
	{-181, -181*337./331, 181, 181*337./331, 2805807}, {153, 153*337./331, -205, -205*337./331, 2805994}, {215, 215*337./331, 138, 138*337./331, 2806991},
	{255, 255*337./331, 18, 18*337./331, 2809646}, {-72, -72*337./331, 245, 245*337./331, 2812114}, {232, 232*337./331, -107, -107*337./331, 2812930},
	{0, 0, 256, 256*337./331, 2814749},
	// Third ring coordinates (~1536 blocks away)
	{-368, -368*337./331, 108, 108*337./331, 14209992}, {-357, -357*337./331, -142, -142*337./331, 14211090}, {-291, -291*337./331, -251, -251*337./331, 14212936},
	{-73, -73*337./331, -377, -377*337./331, 14213660}, {373, 373*337./331, 90, 90*337./331, 14214528}, {382, 382*337./331, -37, -37*337./331, 14214625},
	{323, 323*337./331, 207, 207*337./331, 14215976}, {-108, -108*337./331, 368, 368*337./331, 14218534}, {54, 54*337./331, -380, -380*337./331, 14222625},
	{349, 349*337./331, -160, -160*337./331, 14223602}, {125, 125*337./331, 362, 362*337./331, 14226837}, {237, 237*337./331, 301, 301*337./331, 14227380},
	{-313, -313*337./331, 223, 223*337./331, 14227380}, {-193, -193*337./331, -333, -333*337./331, 14229215}, {277, 277*337./331, -266, -266*337./331, 14231858},
	{-384, -384*337./331, -18, -18*337./331, 14234695}, {-222, -222*337./331, 313, 313*337./331, 14240696}, {175, 175*337./331, -342, -342*337./331, 14243196},
	{0, 0, 384, 384*337./331, 14249670},
	// Fourth ring coordinates (~2048 blocks away)
	{195, 195*337./331, -473, -473*337./331, 44935571}, {-388, -388*337./331, -335, -335*337./331, 44936000}, {-501, -501*337./331, -108, -108*337./331, 44938510},
	{-491, -491*337./331, 145, 145*337./331, 44938660}, {-260, -260*337./331, 440, 440*337./331, 44938660}, {-458, -458*337./331, -229, -229*337./331, 44939175},
	{348, 348*337./331, 374, 374*337./331, 44944238}, {-56, -56*337./331, -509, -509*337./331, 44945031}, {306, 306*337./331, -410, -410*337./331, 44948443},
	{-180, -180*337./331, -480, -480*337./331, 44952476}, {510, 510*337./331, 36, 36*337./331, 44954343}, {-361, -361*337./331, 362, 362*337./331, 44955136},
	{-17, -17*337./331, 511, 511*337./331, 44956595}, {485, 485*337./331, 161, 161*337./331, 44970951}, {-293, -293*337./331, -420, -420*337./331, 44971508},
	{398, 398*337./331, -322, -322*337./331, 44974620}, {245, 245*337./331, 449, 449*337./331, 44977303}, {430, 430*337./331, 276, 276*337./331, 44985780},
	{-512, -512*337./331, 19, 19*337./331, 44987497}, {72, 72*337./331, -507, -507*337./331, 44991897}, {126, 126*337./331, 496, 496*337./331, 44992799},
	{-143, -143*337./331, 491, 491*337./331, 44993829}, {-440, -440*337./331, 262, 262*337./331, 44994795}, {465, 465*337./331, -213, -213*337./331, 45006881},
	{503, 503*337./331, -92, -92*337./331, 45017658}, {0, 0, 512, 512*337./331, 45035996}
};

// TODO: Fix
#if CHECK_DISTANCES
// const double FITNESS = COORDS[RING_STARTING_INDEX][4] > NON_RING_FITNESS ? COORDS[RING_STARTING_INDEX][4] : NON_RING_FITNESS;
const double FITNESS = NON_RING_FITNESS;
#else
const double FITNESS = (RING_STARTING_INDEX <= 1 ) ? 0        /* COORDS[0 ][4] */ :
					   (RING_STARTING_INDEX <= 8 ) ? 174491   /* COORDS[1 ][4] */ :
					   (RING_STARTING_INDEX <= 21) ? 2802142  /* COORDS[8 ][4] */ :
					   (RING_STARTING_INDEX <= 40) ? 14209992 /* COORDS[21][4] */ :
					   								 44935571 /* COORDS[40][4] */ ;
#endif

// The maximum amplitude that all continental octaves after the ith one can return.
const double MAX_OCTAVE_AMPLITUDE_SUMS[] = {990./511 * MAX_PERLIN_VALUE, 734./511 * MAX_PERLIN_VALUE, 606./511 * MAX_PERLIN_VALUE, 478./511 * MAX_PERLIN_VALUE,
											 50./73  * MAX_PERLIN_VALUE, 222./511 * MAX_PERLIN_VALUE, 158./511 * MAX_PERLIN_VALUE,  94./511 * MAX_PERLIN_VALUE,
											 62./511 * MAX_PERLIN_VALUE,  30./511 * MAX_PERLIN_VALUE,  22./511 * MAX_PERLIN_VALUE,   2./73  * MAX_PERLIN_VALUE,
											 10./511 * MAX_PERLIN_VALUE,   6./511 * MAX_PERLIN_VALUE,   4./511 * MAX_PERLIN_VALUE,   2./511 * MAX_PERLIN_VALUE,
											  1./511 * MAX_PERLIN_VALUE,   0};

/* Variable length (and thus runtime-initialized) lookup table. The ith position's jth octave's continental sample must be less than Cdouble[i][j] to still potentially be an 
	nth-ring spawn (assuming no other climates have values extreme enough to influence the spawn fitness calculations, which is true most of the time).
   Derivable by the formula CDOUBLE[i][j] = MAX_OCTAVE_AMPLITUDE_SUMS[j] - (1100 + sqrt(FITNESS - COORDS[i][4]))/10000/(3./2)).*/
double Cdouble[sizeof(COORDS)/sizeof(*COORDS)][sizeof(MAX_OCTAVE_AMPLITUDE_SUMS)/sizeof(*MAX_OCTAVE_AMPLITUDE_SUMS)];

#if CHECK_DISTANCES
// Admittedly copied from Cubiomes' finders.h so I can simply import Cubiomes' noise.c and below, instead of the entire Cubiomes library.
typedef struct {
	int x, z;
} Pos;

// Lookup table containing each point that the first stage of the spawn algorithm checks.
const Pos FIRST_STAGE_POINTS[] = {
	// Zeroth ring
	{0, 0},
	// First ring
	{430, 276}, {-490, 145}, {72, -506}, {-387, -334}, {-143, 491}, {465, -213}, {0, 512},
	// Second ring
	{-1000, -215}, {-981, 290}, {490, 898}, {-774, -669}, {-359, -958}, {144, -1013}, {-722, 725}, {612, -820}, {861, 553}, {1021, 72}, {-286, 983}, {931, -426}, {0, 1024},
	// Third ring
	{-1472, 435}, {-1426, -568}, {-1162, -1003}, {-292, -1507}, {1492, 361}, {1528, -147}, {1292, 829}, {-429, 1474}, {216, -1520}, {1396, -639}, {502, 1451}, {949, 1207}, 
	{-1249, 893}, {-769, -1329}, {1110, -1061}, {-1534, -70}, {-888, 1253}, {702, -1366}, {0, 1536},
	// Fourth ring
	{781, -1892}, {-1549, -1338}, {-2001, -431}, {-1963, 580}, {-1040, 1763}, {-1832, -913}, {1395, 1498}, {-221, -2035}, {1225, -1640}, {-718, -1917}, {2042, 144}, {-1444, 1451},
	{-67, 2046}, {1943, 645}, {-1170, -1680}, {1593, -1286}, {981, 1797}, {1723, 1106}, {-2046, 77}, {289, -2027}, {506, 1984}, {-572, 1966}, {-1759, 1048}, {1862, -852},
	{2015, -365}, {0, 2048}
};
#endif