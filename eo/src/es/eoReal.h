/*
   eoReal.h
// (c) Marc Schoenauer, Maarten Keijzer and GeNeura Team, 2000

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

    Contact: Marc.Schoenauer@polytechnique.fr
             todos@geneura.ugr.es, http://geneura.ugr.es 
             mkeijzer@dhi.dk
*/

#ifndef eoReal_h
#define eoReal_h

//-----------------------------------------------------------------------------

#include <iostream>    // ostream, istream
#include <string>      // string

#include <eoFixedLength.h>

/** eoReal: implementation of simple real-valued chromosome.
  * based on eoFixedLength class
*/
template <class FitT> class eoReal: public eoFixedLength<FitT, double>
{
 public:

  /**
   * (Default) Constructor.
   * @param size Size of the vector
   */
  eoReal(unsigned size = 0, double value = 0.0): 
    eoFixedLength<FitT, double>(size, value) {}
      
  /// My class name.
  string className() const 
    { 
      return "eoReal"; 
    }
  
};

//-----------------------------------------------------------------------------

#endif //eoReal_h
