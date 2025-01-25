// Implementation of Sun Rise and Set Times based on SUNRISET.C by Paul Schlyter 1989-2013.
// Original C code available at https://www.stjarnhimlen.se/comp/sunriset.c (copy in sunriset.c.txt)
// was modified for C++ and OpenKNX.
// For detailed explaination/documentation see https://www.stjarnhimlen.se/comp/riset.html

#pragma once


#include <ctime>
#include <math.h>
#include <stdint.h>


// #define SUN_SUNRISE 0x00
// #define SUN_SUNSET 0x01

/*
struct sTime
{
    int8_t minute;
    int8_t hour;
};

struct sDay
{
    int8_t day;
    int8_t month;
};
*/

class SunRiSet
{


  protected:
    /*
    sTime mSunrise;
    sTime mSunset;
    void calculateSunriseSunset();
    void convertToLocalTime(double iTime, sTime *eTime);
    */

  public:
    static int sunRiseSet(int year, int month, int day, double lon, double lat,
                   double altit, int upper_limb, double *rise, double *set);
  private:
    static void sunPos(double d, double *lon, double *r);
    static void sunRadDec(double d, double *RA, double *dec, double *r);
    static double revolution(double x);
    static double rev180(double x);
    static double GMST0(double d);

    /*
    float mLongitude;
    float mLatitude;

    sTime *getSunInfo(uint8_t iSunInfo);
    void getSunDegree(uint8_t iSunInfo, double iDegree, sTime *eSun);
    */
};

/* A macro to compute the number of days elapsed since 2000 Jan 0.0 */
/* (which is equal to 1999 Dec 31, 0h UT)                           */

#define days_since_2000_Jan_0(y, m, d) \
    (367L * (y) - ((7 * ((y) + (((m) + 9) / 12))) / 4) + ((275 * (m)) / 9) + (d)-730530L)

/* Some conversion factors between radians and degrees */

// #ifndef PI
// #define PI 3.1415926535897932384
// #endif

#define RADEG (180.0 / PI)
#define DEGRAD (PI / 180.0)

/* The trigonometric functions in degrees */

#define sind(x) sin((x)*DEGRAD)
#define cosd(x) cos((x)*DEGRAD)
#define tand(x) tan((x)*DEGRAD)

#define atand(x) (RADEG * atan(x))
#define asind(x) (RADEG * asin(x))
#define acosd(x) (RADEG * acos(x))
#define atan2d(y, x) (RADEG * atan2(y, x))
