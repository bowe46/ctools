/***************************************************************************
 *                       ctskymap - Sky mapping tool                       *
 * ----------------------------------------------------------------------- *
 *  copyright (C) 2011-2016 by Juergen Knoedlseder                         *
 * ----------------------------------------------------------------------- *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/
/**
 * @file ctskymap.cpp
 * @brief Sky mapping tool implementation
 * @author Juergen Knoedlseder
 */

/* __ Includes ___________________________________________________________ */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <cstdio>
#include "ctskymap.hpp"
#include "GTools.hpp"

/* __ Method name definitions ____________________________________________ */
#define G_INIT_MAP                 "ctskymap::init_map(GCTAObservation* obs)"
#define G_BIN_EVENTS                 "ctskymap::bin_events(GCTAObservation*)"

/* __ Debug definitions __________________________________________________ */

/* __ Coding definitions _________________________________________________ */


/*==========================================================================
 =                                                                         =
 =                        Constructors/destructors                         =
 =                                                                         =
 ==========================================================================*/

/***********************************************************************//**
 * @brief Void constructor
 *
 * Constructs an empty sky mapping tool.
 ***************************************************************************/
ctskymap::ctskymap(void) : ctobservation(CTSKYMAP_NAME, CTSKYMAP_VERSION)
{
    // Initialise members
    init_members();

    // Return
    return;
}


/***********************************************************************//**
 * @brief Observations constructor
 *
 * param[in] obs Observation container.
 *
 * Constructs sky mapping tool from an observation container.
 ***************************************************************************/
ctskymap::ctskymap(const GObservations& obs) :
          ctobservation(CTSKYMAP_NAME, CTSKYMAP_VERSION, obs)
{
    // Initialise members
    init_members();

    // Return
    return;
}



/***********************************************************************//**
 * @brief Command line constructor
 *
 * @param[in] argc Number of arguments in command line.
 * @param[in] argv Array of command line arguments.
 *
 * Constructs sky mapping tool using command line arguments for user
 * parameter setting.
 ***************************************************************************/
ctskymap::ctskymap(int argc, char *argv[]) : 
          ctobservation(CTSKYMAP_NAME, CTSKYMAP_VERSION, argc, argv)
{
    // Initialise members
    init_members();

    // Return
    return;
}


/***********************************************************************//**
 * @brief Copy constructor
 *
 * @param[in] app Sky mapping tool.
 ***************************************************************************/
ctskymap::ctskymap(const ctskymap& app) : ctobservation(app)
{
    // Initialise members
    init_members();

    // Copy members
    copy_members(app);

    // Return
    return;
}


/***********************************************************************//**
 * @brief Destructor
 ***************************************************************************/
ctskymap::~ctskymap(void)
{
    // Free members
    free_members();

    // Return
    return;
}


/*==========================================================================
 =                                                                         =
 =                               Operators                                 =
 =                                                                         =
 ==========================================================================*/

/***********************************************************************//**
 * @brief Assignment operator
 *
 * @param[in] app Sky mapping tool.
 * @return Sky mapping tool.
 ***************************************************************************/
ctskymap& ctskymap::operator=(const ctskymap& app)
{
    // Execute only if object is not identical
    if (this != &app) {

        // Copy base class members
        this->ctobservation::operator=(app);

        // Free members
        free_members();

        // Initialise members
        init_members();

        // Copy members
        copy_members(app);

    } // endif: object was not identical

    // Return this object
    return *this;
}


/*==========================================================================
 =                                                                         =
 =                            Public methods                               =
 =                                                                         =
 ==========================================================================*/

/***********************************************************************//**
 * @brief Clear sky mapping tool
 *
 * Clears sky mapping tool.
 ***************************************************************************/
void ctskymap::clear(void)
{
    // Free members
    free_members();
    this->ctool::free_members();
    this->ctobservation::free_members();

    // Clear base class (needed to conserve tool name and version)
    this->GApplication::clear();

    // Initialise members
    this->ctool::init_members();
    this->ctobservation::init_members();
    init_members();

    // Write header into logger
    log_header();

    // Return
    return;
}


/***********************************************************************//**
 * @brief Run the sky mapping tool
 *
 * Generates a sky map from event list by looping over all unbinned CTA
 * observation in the observation container and filling all events into
 * a sky map.
 ***************************************************************************/
void ctskymap::run(void)
{
    // Switch screen logging on in debug mode
    if (logDebug()) {
        log.cout(true);
    }

    // Get parameters
    get_parameters();

    // Write input observation container into logger
    log_observations(NORMAL, m_obs, "Input observation");

    // Write header into logger
    log_header1(TERSE, gammalib::number("Map observation", m_obs.size()));

    // Loop over all unbinned CTA observations in the container
    for (GCTAObservation* obs = first_unbinned_observation(); obs != NULL;
         obs = next_unbinned_observation()) {

        // Map events into sky map
        map_events(obs);
            
    } // endfor: looped over observations

    // Optionally publish sky map
    if (m_publish) {
        publish();
    }

    // Return
    return;
}


/***********************************************************************//**
 * @brief Save sky map
 *
 * Saves the sky map into a FITS file. The FITS file name is specified by the
 * @p outname parameter.
 ***************************************************************************/
void ctskymap::save(void)
{
    // Write header
    log_header1(TERSE, "Save sky map");

    // Get sky map filename
    m_outmap  = (*this)["outmap"].filename();

    // Save sky map if filename and the map are not empty
    if (!m_outmap.is_empty() && !m_skymap.is_empty()) {

        // Create empty FITS file
        GFits fits;

        // Write sky map into FITS file
        m_skymap.write(fits);

        // Save FITS file to disk
        fits.saveto(m_outmap, clobber());

    }

    // Write into logger what has been done
    std::string fname = (m_outmap.is_empty()) ? "NONE" : m_outmap.url();
    if (m_skymap.is_empty()) {
        fname.append(" (map is empty, no file created)");
    }
    log_value(NORMAL, "Sky map file", fname);

    // Return
    return;
}


/***********************************************************************//**
 * @brief Publish sky map
 *
 * @param[in] name Sky map name.
 ***************************************************************************/
void ctskymap::publish(const std::string& name)
{
    // Write header into logger
    log_header1(TERSE, "Publish sky map");

    // Set default name is user name is empty
    std::string user_name(name);
    if (user_name.empty()) {
        user_name = CTSKYMAP_NAME;
    }

    // Write sky map name into logger
    log_value(NORMAL, "Sky map name", user_name);

    // Publish sky map
    m_skymap.publish(user_name);

    // Return
    return;
}


/*==========================================================================
 =                                                                         =
 =                             Private methods                             =
 =                                                                         =
 ==========================================================================*/

/***********************************************************************//**
 * @brief Initialise class members
 ***************************************************************************/
void ctskymap::init_members(void)
{
    // Initialise members
    m_skymap.clear();
    m_emin    = 0.0;
    m_emax    = 0.0;
    m_publish = false;
    m_chatter = static_cast<GChatter>(2);

    // Return
    return;
}


/***********************************************************************//**
 * @brief Copy class members
 *
 * @param[in] app Application.
 ***************************************************************************/
void ctskymap::copy_members(const ctskymap& app)
{
    // Copy attributes
    m_skymap  = app.m_skymap;
    m_emin    = app.m_emin;
    m_emax    = app.m_emax;
    m_publish = app.m_publish;
    m_chatter = app.m_chatter;

    // Return
    return;
}


/***********************************************************************//**
 * @brief Delete class members
 ***************************************************************************/
void ctskymap::free_members(void)
{
    // Return
    return;
}


/***********************************************************************//**
 * @brief Get application parameters
 *
 * Get all task parameters from parameter file or (if required) by querying
 * the user. Most parameters are only required if no observation exists so
 * far in the observation container. In this case, a single CTA observation
 * will be added to the container, using the definition provided in the
 * parameter file.
 ***************************************************************************/
void ctskymap::get_parameters(void)
{
    // Setup observations from "inobs" parameter. Do not request response
    // information and do not accept counts cubes.
    setup_observations(m_obs, false, true, false);

    // Create sky map based on task parameters
    m_skymap = create_map(m_obs);

    // Get remaining parameters
    m_emin    = (*this)["emin"].real();
    m_emax    = (*this)["emax"].real();
    m_publish = (*this)["publish"].boolean();
    m_chatter = static_cast<GChatter>((*this)["chatter"].integer());

    // Read ahead parameters
    if (read_ahead()) {
        m_outmap  = (*this)["outmap"].filename();
    }

    // Write parameters into logger
    log_parameters(TERSE);

    // Return
    return;
}


/***********************************************************************//**
 * @brief Map events into a sky map
 *
 * @param[in] obs CTA observation.
 *
 * @exception GException::no_list
 *            No event list found in observation.
 * @exception GCTAException::no_pointing
 *            No valid CTA pointing found.
 *
 * This method maps the events found in a CTA events list into a sky map.
 ***************************************************************************/
void ctskymap::map_events(GCTAObservation* obs)
{
    // Make sure that the observation holds a CTA event list. If this
    // is not the case then throw an exception.
    if (dynamic_cast<const GCTAEventList*>(obs->events()) == NULL) {
        throw GException::no_list(G_BIN_EVENTS);
    }

    // Setup energy range covered by data
    GEnergy  emin;
    GEnergy  emax;
    GEbounds ebds;
    emin.TeV(m_emin);
    emax.TeV(m_emax);

    // Initialise binning statistics
    int num_outside_map    = 0;
    int num_outside_erange = 0;
    int num_in_map         = 0;

    // Fill sky map
    GCTAEventList* events = static_cast<GCTAEventList*>
                                       (const_cast<GEvents*>(obs->events()));
    for (int i = 0; i < events->size(); ++i) {

        // Get event
        GCTAEventAtom* event = (*events)[i];

        // Skip if energy is out of range
        if (event->energy() < emin || event->energy() > emax) {
            num_outside_erange++;
            continue;
        }

        // Determine sky pixel
        GCTAInstDir* inst  = (GCTAInstDir*)&(event->dir());
        GSkyDir      dir   = inst->dir();
        GSkyPixel    pixel = m_skymap.dir2pix(dir);

        // Skip if pixel is out of range
        if (pixel.x() < -0.5 || pixel.x() > (m_skymap.nx() - 0.5) ||
            pixel.y() < -0.5 || pixel.y() > (m_skymap.ny() - 0.5)) {
            num_outside_map++;
            continue;
        }

        // Fill event in skymap
        m_skymap(pixel, 0) += 1.0;
        num_in_map++;

    } // endfor: looped over all events

    // Log binning results
    log_value(NORMAL, "Events in list", obs->events()->size());
    log_value(NORMAL, "Events in map", num_in_map);
    log_value(NORMAL, "Events outside map area", num_outside_map);
    log_value(NORMAL, "Events outside energies", num_outside_erange);

    // Write sky map into header
    log_header2(EXPLICIT, "Sky map");
    log_string(EXPLICIT, m_skymap.print(m_chatter));

    // Return
    return;
}
