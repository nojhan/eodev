// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// eoStat.h
// (c) Marc Schoenauer, Maarten Keijzer and GeNeura Team, 2000
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

#ifndef _eoStat_h
#define _eoStat_h

#include <numeric> // accumulate

#include <eoFunctor.h>
#include <utils/eoParam.h>
#include <eoPop.h>
#include <eoScalarFitnessAssembled.h>
#include <eoParetoFitness.h>

/**
  Base class for all statistics that need to be calculated
  over the (unsorted) population 
  (I guess it is not really necessary? MS. 
  Depstd::ends, there might be reasons to have a stat that is not an eoValueParam,
  but maybe I'm just kidding myself, MK)
*/
template <class EOT>
class eoStatBase : public eoUF<const eoPop<EOT>&, void>
{
public:
  virtual void lastCall(const eoPop<EOT>&) {}
};

/**
  The actual class that will be used as base for all statistics
  that need to be calculated over the (unsorted) population
  It is an eoStatBase AND an eoValueParam so it can be used in Monitors.
*/
template <class EOT, class T>
class eoStat : public eoValueParam<T>, public eoStatBase<EOT>
{
public :
    eoStat(T _value, std::string _description) : eoValueParam<T>(_value, _description) {}
};

/**
  Base class for statistics calculated over a sorted snapshot of the population
*/
template <class EOT>
class eoSortedStatBase : public eoUF<const std::vector<const EOT*>&, void>
{
public:
  virtual void lastCall(const std::vector<const EOT*>&) {}
};

/**
  The actual class that will be used as base for all statistics
  that need to be calculated over the sorted population
  It's an eoSortedStatBase AND an eoValueParam so it can be used in Monitors.
*/
template <class EOT, class ParamType>
class eoSortedStat : public eoSortedStatBase<EOT>, public eoValueParam<ParamType>
{
public :
  eoSortedStat(ParamType _value, std::string _desc) : eoValueParam<ParamType>(_value, _desc) {}
};

/**
   Average fitness of a population. Fitness can be: 
   - double
   - eoMinimizingFitness or eoMaximizingFitness 
   - eoScalarFitnessAssembled: 
     Specify in the constructor, for which fitness term (index) the average should be evaluated.
   - eoParetoFitness:
     The average of each objective is evaluated.
*/
#ifdef _MSC_VER
template <class EOT> class eoAverageStat : public eoStat<EOT, EOT::Fitness>
#else
template <class EOT> class eoAverageStat : public eoStat<EOT, typename EOT::Fitness>
#endif
{
public :
    typedef typename EOT::Fitness fitness_type;
#ifdef _MSC_VER 
    eoAverageStat(std::string _description = "Average Fitness") 
      : eoStat<EOT, EOT::Fitness>(fitness_type(), _description), whichFitnessTerm() {}
    eoAverageStat(unsigned _whichTerm, std::string _description = "Average Fitness") 
      : eoStat<EOT, EOT::Fitness>(fitness_type(), _description), whichFitnessTerm(_whichTerm) {}
#else
    eoAverageStat(std::string _description = "Average Fitness") 
      : eoStat<EOT, typename EOT::Fitness>(fitness_type(), _description), whichFitnessTerm() {}
    eoAverageStat(unsigned _whichTerm, std::string _description = "Average Fitness") 
      : eoStat<EOT, typename EOT::Fitness>(fitness_type(), _description), whichFitnessTerm(_whichTerm) {}
#endif

    static fitness_type sumFitness(double _sum, const EOT& _eot)
    {
        _sum += _eot.fitness();
        return _sum;
    }

    eoAverageStat(double _value, std::string _desc) : eoStat<EOT, double>(_value, _desc) {}

    virtual void operator()(const eoPop<EOT>& _pop)
    {
#ifdef _MSC_VER
        doit(_pop, EOT::Fitness()); // specializations for scalar and std::vector
#else
        doit(_pop, typename EOT::Fitness()); // specializations for scalar and std::vector
#endif
    }
private :
    
    // Specialization for pareto fitness 
    template <class T>
    void doit(const eoPop<EOT>& _pop, eoParetoFitness<T>)
    {
      value().clear();
      value().resize(_pop[0].fitness().size(), 0.0);

      for (unsigned o = 0; o < value().size(); ++o)
      {
        for (unsigned i = 0; i < _pop.size(); ++i)
        {
          value()[o] += _pop[i].fitness()[o];
        }

        value()[o] /= _pop.size();
      }
    }
    
    // Specialization for eoScalarFitnessAssembled
    template <class ScalarType, class Compare>
    void doit(const eoPop<EOT>& _pop, eoScalarFitnessAssembled<ScalarType, Compare>)
    {
      
      if ( whichFitnessTerm >= _pop[0].fitness().size() )
      	throw std::logic_error("Fitness term requested out of range");
      
      double result =0.0;      
      for (typename eoPop<EOT>::const_iterator it = _pop.begin(); it != _pop.end(); ++it)
	result+= it->fitness()[whichFitnessTerm];
      value().clear();
      value() = result / _pop.size();
    }
    
    // Default behavior
    template <class T>
    void doit(const eoPop<EOT>& _pop, T)
    {
        fitness_type v = std::accumulate(_pop.begin(), _pop.end(), fitness_type(0.0), eoAverageStat::sumFitness);

        value() = v / _pop.size();
    }

    // Store an index of the fitness term to be evaluated in eoScalarFitnessAssembled
    unsigned whichFitnessTerm;
};

/**
    Average fitness + Std. dev. of a population, fitness needs to be scalar.
*/
template <class EOT>
class eoSecondMomentStats : public eoStat<EOT, std::pair<double, double> >
{
public :
    typedef typename EOT::Fitness fitness_type;
    
    typedef std::pair<double, double> SquarePair;
    eoSecondMomentStats(std::string _description = "Average & Stdev") : eoStat<EOT, SquarePair>(std::make_pair(0.0,0.0), _description) {}

    static SquarePair sumOfSquares(SquarePair _sq, const EOT& _eo)
    {
        double fitness = _eo.fitness();

        _sq.first += fitness;
        _sq.second += fitness * fitness;
        return _sq;
    }

    virtual void operator()(const eoPop<EOT>& _pop)
    {
        SquarePair result = std::accumulate(_pop.begin(), _pop.end(), std::make_pair(0.0, 0.0), eoSecondMomentStats::sumOfSquares);

        double n = _pop.size();
        value().first = result.first / n; // average
        value().second = sqrt( (result.second - n * value().first * value().first) / (n - 1.0)); // stdev
    }
};

/**
    The n_th element fitness in the population (see eoBestFitnessStat)
*/
#ifdef _MSC_VER 
template <class EOT>
class eoNthElementFitnessStat : public eoSortedStat<EOT, EOT::Fitness >
#else
template <class EOT>
class eoNthElementFitnessStat : public eoSortedStat<EOT, typename EOT::Fitness >
#endif
{
public :
    typedef typename EOT::Fitness Fitness;

    eoNthElementFitnessStat(unsigned _whichElement, std::string _description = "nth element fitness") 
      : eoSortedStat<EOT, Fitness>(Fitness(), _description), whichElement(_whichElement) {}
    eoNthElementFitnessStat(unsigned _whichElement, unsigned _whichTerm, std::string _description = "nth element fitness") 
      : eoSortedStat<EOT, Fitness>(Fitness(), _description), whichElement(_whichElement), whichFitnessTerm(_whichTerm) {}

    virtual void operator()(const std::vector<const EOT*>& _pop)
    {
        if (whichElement > _pop.size())
            throw std::logic_error("fitness requested of element outside of pop");

        doit(_pop, Fitness());
    }

private :
    
    struct CmpFitness
    {
      CmpFitness(unsigned _whichElement, bool _maxim) : whichElement(_whichElement), maxim(_maxim) {}

      bool operator()(const EOT* a, const EOT* b)
      {
        if (maxim)
          return a->fitness()[whichElement] > b->fitness()[whichElement];

        return a->fitness()[whichElement] < b->fitness()[whichElement];
      }

      unsigned whichElement;
      bool maxim;
    };

    // Specialization for eoParetoFitness
    template <class T>
    void doit(const eoPop<EOT>& _pop, eoParetoFitness<T>)
    {
      typedef typename EOT::Fitness::fitness_traits traits;

      value().resize(traits::nObjectives());

      // copy of pointers, what the heck
      std::vector<const EOT*> tmp_pop = _pop;

      for (unsigned o = 0; o < value().size(); ++o)
      {
        typename std::vector<const EOT*>::iterator nth = tmp_pop.begin() + whichElement;
        std::nth_element(tmp_pop.begin(), nth, tmp_pop.end(), CmpFitness(o, traits::maximizing(o)));
        value()[o] = (*nth)->fitness()[o];
      }
    }
    
    // Specialization for eoScalarFitnessAssembled
    template <class ScalarType, class Compare>
    void doit(const eoPop<EOT>& _pop, eoScalarFitnessAssembled<ScalarType, Compare>)
    {
      if ( whichFitnessTerm >= _pop[0].fitness().size() )
	throw std::logic_error("Fitness term requested out of range");
      value().clear();
      value() = _pop[whichElement]->fitness()[whichFitnessTerm];
    }

    // for everything else
    template <class T>
    void doit(const std::vector<const EOT*>& _pop, T)
    {
      value() = _pop[whichElement]->fitness();
    }

    unsigned whichElement;
    unsigned whichFitnessTerm;
};

/* Actually, you shouldn't need to sort the population to get the best fitness
   MS - 17/11/00

   But then again, if another stat needs sorted fitness anyway, getting the best
   out would be very fast.
   MK - 09/01/03
   
template <class EOT>
class eoBestFitnessStat : public eoStat<EOT, typename EOT::Fitness >
{
public :
    typedef typename EOT::Fitness Fitness;

    eoBestFitnessStat(std::string _description = "Best Fitness") :
      eoStat<EOT, Fitness>(Fitness(), _description) {}

    virtual void operator()(const eoPop<EOT>& _pop)
    {
        value() = _pop.nth_element_fitness(0);
    }

};
*/

/**
   Best fitness of a population. Fitness can be: 
   - double
   - eoMinimizingFitness or eoMaximizingFitness 
   - eoScalarFitnessAssembled: 
     Best individual is found according to its fitness, 
     specify in the constructor which fitness term of this individual should then be stored.
   - eoParetoFitness:
*/

#ifdef _MSC_VER
template <class EOT>
class eoBestFitnessStat : public eoStat<EOT, EOT::Fitness>
#else
template <class EOT>
class eoBestFitnessStat : public eoStat<EOT, typename EOT::Fitness>
#endif
{
public :
    typedef typename EOT::Fitness Fitness;

    eoBestFitnessStat(std::string _description = "Best ") 
      : eoStat<EOT, Fitness>(Fitness(), _description) {}
    eoBestFitnessStat(unsigned _whichTerm, std::string _description = "Best ") 
      : eoStat<EOT, Fitness>(Fitness(), _description), whichFitnessTerm(_whichTerm) {}
  
    void operator()(const eoPop<EOT>& _pop){
#ifdef _MSC_VER
        doit(_pop, EOT::Fitness() ); // specializations for scalar and std::vector
#else
        doit(_pop, typename EOT::Fitness()); // specializations for scalar and std::vector
#endif
    }

private :

    struct CmpFitness
    {
      CmpFitness(unsigned _which, bool _maxim) : which(_which), maxim(_maxim) {}

      bool operator()(const EOT& a, const EOT& b)
      {
        if (maxim)
          return a.fitness()[which] < b.fitness()[which];

        return a.fitness()[which] > b.fitness()[which];
      }

      unsigned which;
      bool maxim;
    };

    // Specialization for pareto fitness
    template <class T>
    void doit(const eoPop<EOT>& _pop, eoParetoFitness<T>)
    {
      typedef typename EOT::Fitness::fitness_traits traits;
      value().resize(traits::nObjectives());

      for (unsigned o = 0; o < traits::nObjectives(); ++o)
      {
        typename eoPop<EOT>::const_iterator it = std::max_element(_pop.begin(), _pop.end(), CmpFitness(o, traits::maximizing(o)));
        value()[o] = it->fitness()[o];
      }
    }
    
    // Specialization for eoScalarFitnessAssembled
    template <class ScalarType, class Compare>
    void doit(const eoPop<EOT>& _pop, eoScalarFitnessAssembled<ScalarType, Compare>){

      if ( whichFitnessTerm >= _pop[0].fitness().size() )
	throw std::logic_error("Fitness term requested out of range");
      value().clear();
      value() = _pop.best_element().fitness()[whichFitnessTerm];      
    }
  
    // default
    template<class T>
    void doit(const eoPop<EOT>& _pop, T)
    { // find the largest elements
      value() = _pop.best_element().fitness();
    }

    unsigned whichFitnessTerm;
};

template <class EOT>
class eoDistanceStat : public eoStat<EOT, double>
{
public :
    eoDistanceStat(std::string _name = "distance") : eoStat<EOT, double>(0.0, _name) {}

    template <class T>
    double distance(T a, T b)
    {
        T res = a-b;
        return res < 0? -res : res;
    }

    double distance(bool a, bool b)
    {
        return (a==b)? 0 : 1;
    }

    void operator()(const eoPop<EOT>& _pop)
    {
        double& v = value();
        v = 0.0;

        for (unsigned i = 0; i < _pop.size(); ++i)
        {
            for (unsigned j = 0; j < _pop.size(); ++j)
            {
                for (unsigned k = 0; k < _pop[i].size(); ++k)
                {
                    v += distance(_pop[i][k], _pop[j][k]);
                }
            }
        }

        double sz = _pop.size();
        v /= sz * sz * _pop[0].size();
    }
};



/*
template <class EOT>
class eoStdevStat : public eoStat<EOT, double >
{
public :
    typedef typename eoSecondMomentStats<EOT>::SquarePair SquarePair;

    eoStdevStat(std::string _description = "Stdev") : eoStat<EOT, double>(0.0, _description) {}

    virtual void operator()(const eoPop<EOT>& _pop)
    {
        SquarePair result = std::accumulate(pop.begin(), pop.end(), std::make_pair(0.0, 0.0), eoSecondMomentStats::sumOfSquares);
    
        double n = pop.size();
        value() = sqrt( (result.second - (result.first / n)) / (n - 1.0)); // stdev
    }
};
*/
#endif
