#ifndef _doAlgo_h
#define _doAlgo_h

#include <eoAlgo.h>

template < typename D >
class doAlgo : public eoAlgo< typename D::EOType >
{
    //! Alias for the type
    typedef typename D::EOType EOT;

public:
    virtual ~doAlgo(){}
};

#endif // !_doAlgo_h
