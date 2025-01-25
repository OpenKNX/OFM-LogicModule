// Implementation of Sun Rise and Set Times based on SUNRISET.C by Paul Schlyter 1989-2013.
// Original C code available at https://www.stjarnhimlen.se/comp/sunriset.c (copy in sunriset.c.txt)
// was modified for C++ and OpenKNX.
// For detailed explaination/documentation see https://www.stjarnhimlen.se/comp/riset.html

#include "SunRiSet.h"
#include "Arduino.h"
#include <ctime>


/*
void SunRiSet::convertToLocalTime(double iTime, sTime *eTime)
{
    eTime->hour = (int)floor(iTime);
    eTime->minute = (int)(60 * (iTime - floor(iTime)));
    eTime->hour += mTimezone + ((mIsSummertime) ? 1 : 0);
}

void SunRiSet::calculateSunriseSunset()
{
    double rise, set;
    // sunrise/sunset calculation
    sunRiseSet(getYear(), getMonth(), getDay(),
               mLongitude, mLatitude, -35.0 / 60.0, 1, &rise, &set);
    convertToLocalTime(rise, &mSunrise);
    convertToLocalTime(set, &mSunset);
}

sTime *SunRiSet::getSunInfo(uint8_t iSunInfo)
{
    if (iSunInfo == SUN_SUNRISE)
        return &mSunrise;
    else if (iSunInfo == SUN_SUNSET)
        return &mSunset;
    else
        return NULL;
}

void SunRiSet::getSunDegree(uint8_t iSunInfo, double iDegree, sTime *eSun)
{
    double rise, set;
    // sunrise/sunset calculation
    sunRiseSet(getYear(), getMonth(), getDay(),
               mLongitude, mLatitude, iDegree, 0, &rise, &set);
    if (iSunInfo == SUN_SUNRISE)
        convertToLocalTime(rise, eSun);
    else if (iSunInfo == SUN_SUNSET)
        convertToLocalTime(set, eSun);
}
*/


#pragma region SUNRISET

/***************************************************************************/
/* Note: year,month,date = calendar date, 1801-2099 only.             */
/*       Eastern longitude positive, Western longitude negative       */
/*       Northern latitude positive, Southern latitude negative       */
/*       The longitude value IS critical in this function!            */
/*       altit = the altitude which the Sun should cross              */
/*               Set to -35/60 degrees for rise/set, -6 degrees       */
/*               for civil, -12 degrees for nautical and -18          */
/*               degrees for astronomical twilight.                   */
/*         upper_limb: non-zero -> upper limb, zero -> center         */
/*               Set to non-zero (e.g. 1) when computing rise/set     */
/*               times, and to zero when computing start/end of       */
/*               twilight.                                            */
/*        *rise = where to store the rise time                        */
/*        *set  = where to store the set  time                        */
/*                Both times are relative to the specified altitude,  */
/*                and thus this function can be used to compute       */
/*                various twilight times, as well as rise/set times   */
/* Return value:  0 = sun rises/sets this day, times stored at        */
/*                    *trise and *tset.                               */
/*               +1 = sun above the specified "horizon" 24 hours.     */
/*                    *trise set to time when the sun is at south,    */
/*                    minus 12 hours while *tset is set to the south  */
/*                    time plus 12 hours. "Day" length = 24 hours     */
/*               -1 = sun is below the specified "horizon" 24 hours   */
/*                    "Day" length = 0 hours, *trise and *tset are    */
/*                    both set to the time when the sun is at south.  */
/*                                                                    */
/**********************************************************************/
int SunRiSet::sunRiseSet(int year, int month, int day, double lon, double lat,
                      double altit, int upper_limb, double *trise, double *tset)
{
    double d,    /* Days since 2000 Jan 0.0 (negative before) */
        sr,      /* Solar distance, astronomical units */
        sRA,     /* Sun's Right Ascension */
        sdec,    /* Sun's declination */
        sradius, /* Sun's apparent radius */
        t,       /* Diurnal arc */
        tsouth,  /* Time when Sun is at south */
        sidtime; /* Local sidereal time */

    int rc = 0; /* Return cde from function - usually 0 */

    /* Compute d of 12h local mean solar time */
    d = days_since_2000_Jan_0(year, month, day) + 0.5 - lon / 360.0;

    /* Compute the local sidereal time of this moment */
    sidtime = revolution(GMST0(d) + 180.0 + lon);

    /* Compute Sun's RA, Decl and distance at this moment */
    sunRadDec(d, &sRA, &sdec, &sr);

    /* Compute time when Sun is at south - in hours UT */
    tsouth = 12.0 - rev180(sidtime - sRA) / 15.0;

    /* Compute the Sun's apparent radius in degrees */
    sradius = 0.2666 / sr;

    /* Do correction to upper limb, if necessary */
    if (upper_limb)
        altit -= sradius;

    /* Compute the diurnal arc that the Sun traverses to reach */
    /* the specified altitude altit: */
    {
        double cost;
        cost = (sind(altit) - sind(lat) * sind(sdec)) /
               (cosd(lat) * cosd(sdec));
        if (cost >= 1.0)
            rc = -1, t = 0.0; /* Sun always below altit */
        else if (cost <= -1.0)
            rc = +1, t = 12.0; /* Sun always above altit */
        else
            t = acosd(cost) / 15.0; /* The diurnal arc, hours */
    }

    /* Store rise and set times - in hours UT */
    *trise = tsouth - t;
    *tset = tsouth + t;

    return rc;
}

/******************************************************/
/* Computes the Sun's ecliptic longitude and distance */
/* at an instant given in d, number of days since     */
/* 2000 Jan 0.0.  The Sun's ecliptic latitude is not  */
/* computed, since it's always very near 0.           */
/******************************************************/
void SunRiSet::sunPos(double d, double *lon, double *r)
{
    double M, /* Mean anomaly of the Sun */
        w,    /* Mean longitude of perihelion */
              /* Note: Sun's mean longitude = M + w */
        e,    /* Eccentricity of Earth's orbit */
        E,    /* Eccentric anomaly */
        x, y, /* x, y coordinates in orbit */
        v;    /* True anomaly */

    /* Compute mean elements */
    M = revolution(356.0470 + 0.9856002585 * d);
    w = 282.9404 + 4.70935E-5 * d;
    e = 0.016709 - 1.151E-9 * d;

    /* Compute true longitude and radius vector */
    E = M + e * RADEG * sind(M) * (1.0 + e * cosd(M));
    x = cosd(E) - e;
    y = sqrt(1.0 - e * e) * sind(E);
    *r = sqrt(x * x + y * y); /* Solar distance */
    v = atan2d(y, x);         /* True anomaly */
    *lon = v + w;             /* True solar longitude */
    if (*lon >= 360.0)
        *lon -= 360.0; /* Make it 0..360 degrees */
}

/******************************************************/
/* Computes the Sun's equatorial coordinates RA, Decl */
/* and also its distance, at an instant given in d,   */
/* the number of days since 2000 Jan 0.0.             */
/******************************************************/
void SunRiSet::sunRadDec(double d, double *RA, double *dec, double *r)
{
    double lon, obl_ecl, x, y, z;

    /* Compute Sun's ecliptical coordinates */
    sunPos(d, &lon, r);

    /* Compute ecliptic rectangular coordinates (z=0) */
    x = *r * cosd(lon);
    y = *r * sind(lon);

    /* Compute obliquity of ecliptic (inclination of Earth's axis) */
    obl_ecl = 23.4393 - 3.563E-7 * d;

    /* Convert to equatorial rectangular coordinates - x is unchanged */
    z = y * sind(obl_ecl);
    y = y * cosd(obl_ecl);

    /* Convert to spherical coordinates */
    *RA = atan2d(y, x);
    *dec = atan2d(z, sqrt(x * x + y * y));
}

/******************************************************************/
/* This function reduces any angle to within the first revolution */
/* by subtracting or adding even multiples of 360.0 until the     */
/* result is >= 0.0 and < 360.0                                   */
/******************************************************************/

#define INV360 (1.0 / 360.0)

/*****************************************/
/* Reduce angle to within 0..360 degrees */
/*****************************************/
double SunRiSet::revolution(double x)
{
    return (x - 360.0 * floor(x * INV360));
}

/*********************************************/
/* Reduce angle to within +180..+180 degrees */
/*********************************************/
double SunRiSet::rev180(double x)
{
    return (x - 360.0 * floor(x * INV360 + 0.5));
}

/*******************************************************************/
/* This function computes GMST0, the Greenwich Mean Sidereal Time  */
/* at 0h UT (i.e. the sidereal time at the Greenwhich meridian at  */
/* 0h UT).  GMST is then the sidereal time at Greenwich at any     */
/* time of the day.  I've generalized GMST0 as well, and define it */
/* as:  GMST0 = GMST - UT  --  this allows GMST0 to be computed at */
/* other times than 0h UT as well.  While this sounds somewhat     */
/* contradictory, it is very practical:  instead of computing      */
/* GMST like:                                                      */
/*                                                                 */
/*  GMST = (GMST0) + UT * (366.2422/365.2422)                      */
/*                                                                 */
/* where (GMST0) is the GMST last time UT was 0 hours, one simply  */
/* computes:                                                       */
/*                                                                 */
/*  GMST = GMST0 + UT                                              */
/*                                                                 */
/* where GMST0 is the GMST "at 0h UT" but at the current moment!   */
/* Defined in this way, GMST0 will increase with about 4 min a     */
/* day.  It also happens that GMST0 (in degrees, 1 hr = 15 degr)   */
/* is equal to the Sun's mean longitude plus/minus 180 degrees!    */
/* (if we neglect aberration, which amounts to 20 seconds of arc   */
/* or 1.33 seconds of time)                                        */
/*                                                                 */
/*******************************************************************/

double SunRiSet::GMST0(double d)
{
    double sidtim0;
    /* Sidtime at 0h UT = L (Sun's mean longitude) + 180.0 degr  */
    /* L = M + w, as defined in sunpos().  Since I'm too lazy to */
    /* add these numbers, I'll let the C compiler do it for me.  */
    /* Any decent C compiler will add the constants at compile   */
    /* time, imposing no runtime or code overhead.               */
    sidtim0 = revolution((180.0 + 356.0470 + 282.9404) +
                         (0.9856002585 + 4.70935E-5) * d);
    return sidtim0;
} /* GMST0 */

#pragma endregion
