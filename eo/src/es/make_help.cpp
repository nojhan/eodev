// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// make_help.h
// (c) Maarten Keijzer, Marc Schoenauer and GeNeura Team, 2001
/* 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: todos@geneura.ugr.es, http://geneura.ugr.es
             Marc.Schoenauer@polytechnique.fr
             mkeijzer@dhi.dk
 */
//-----------------------------------------------------------------------------
#include <utils/eoParser.h>
#include <fstream.h>

/** Generation of the status file, and output of the help message if needed
 *
 * MUST be called after ALL parameters have been read in order to list them
 *
 * Warning: this is a plain .cpp file and shoudl NOT be included anywhere, 
 * but compiled separately and stored in a library.
 */
void make_help(eoParser & _parser)
{
    // name of the "status" file where all actual parameter values will be saved
    string str_status = _parser.ProgramName() + ".status"; // default value
    eoValueParam<string>& statusParam = _parser.createParam(str_status, "status","Status file",'\0', "Persistence" );

   // do the following AFTER ALL PARAMETERS HAVE BEEN PROCESSED
   // i.e. in case you need parameters somewhere else, postpone these
    if (_parser.userNeedsHelp())
      {
        _parser.printHelp(cout);
        exit(1);
      }
    if (statusParam.value() != "")
      {
	ofstream os(statusParam.value().c_str());
	os << _parser;		// and you can use that file as parameter file
      }
}
