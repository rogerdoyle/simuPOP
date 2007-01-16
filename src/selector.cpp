/***************************************************************************
 *   Copyright (C) 2004 by Bo Peng                                         *
 *   bpeng@rice.edu                                                        *
 *                                                                         *
 *   $LastChangedDate: 2006-02-21 15:27:25 -0600 (Tue, 21 Feb 2006) $
 *   $Rev: 191 $
 *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "selector.h"

namespace simuPOP
{
	double mapSelector::indFitness(individual * ind)
	{
		string key;

		for(vectoru::iterator loc=m_loci.begin(); loc!=m_loci.end(); ++loc)
		{
			/// get genotype of ind
			Allele a = ind->allele(*loc, 0);
			Allele b = ind->allele(*loc, 1);

			if( loc != m_loci.begin() )
				key += '|';
			if( ! m_phase && a > b )			  // ab=ba
				key +=  toStr(static_cast<int>(b)) + "-" + toStr(static_cast<int>(a));
			else
				key +=  toStr(static_cast<int>(a)) + "-" + toStr(static_cast<int>(b));
		}

		strDict::iterator pos = m_dict.find(key);

		DBG_ASSERT( pos != m_dict.end(), ValueError,
			"No fitness value for genotype " + key);

		return( pos->second);
	}

	/// currently assuming diploid
	double maSelector::indFitness(individual * ind)
	{
		UINT index = 0;
		bool singleST = m_wildtype.size() == 1;
		for(vectoru::iterator loc=m_loci.begin(); loc!=m_loci.end(); ++loc)
		{
			/// get genotype of ind
			Allele a = ind->allele(*loc, 0);
			Allele b = ind->allele(*loc, 1);

			int numWildtype=0;

			// count number of wildtype
			// this improve the performance a little bit
			if(singleST)
			{
				numWildtype = (a==m_wildtype[0]) + (b==m_wildtype[0]);
			}
			else
			{
				if(find(m_wildtype.begin(), m_wildtype.end(), a) != m_wildtype.end() )
					numWildtype ++;

				if(find(m_wildtype.begin(), m_wildtype.end(), b) != m_wildtype.end() )
					numWildtype ++;
			}

			index = index*3 + 2-numWildtype;
		}
		return m_fitness[index];
	}

	double mlSelector::indFitness(individual * ind)
	{
		if(m_mode == SEL_Multiplicative )
		{
			double fit = 1;
			for(vectorop::iterator s = m_selectors.begin(), sEnd=m_selectors.end();
				s != sEnd; ++s)
			fit *= static_cast<selector* >(*s)->indFitness(ind);
			return fit;
		}
		else if(m_mode == SEL_Additive )
		{
			double fit = 1;
			for(vectorop::iterator s = m_selectors.begin(), sEnd=m_selectors.end();
				s != sEnd; ++s)
			fit -= 1 - static_cast<selector* >(*s)->indFitness(ind);
			return fit<0?0.:fit;
		}
		else if(m_mode == SEL_Heterogeneity)
			/// fixme
		{
			double fit = 1;
			for(vectorop::iterator s = m_selectors.begin(), sEnd=m_selectors.end();
				s != sEnd; ++s)
			fit *= 1 - static_cast<selector* >(*s)->indFitness(ind);
			return 1-fit;
		}
		// this is the case for SEL_none.
		return 1.0;
	}

	double pySelector::indFitness(individual * ind)
	{
		if( m_len == 0)
		{
			m_len = m_loci.size() * ind->ploidy();
			m_alleles.resize( m_len);
#ifdef SIMUMPI
			m_numArray = Allele_Vec_As_NumArray( m_alleles.begin(), m_alleles.end(),
				m_alleles.size(), m_alleles.size(), 0, m_alleles.size());
#else
			m_numArray = Allele_Vec_As_NumArray( m_alleles.begin(), m_alleles.end() );
#endif
		}

		DBG_FAILIF( static_cast<size_t>(m_len) != ind->ploidy() * m_loci.size(),
			SystemError,
			"Length of m_len is wrong. Have you changed pop type?" );

		UINT pEnd = ind->ploidy();
		for(size_t i=0, iEnd=m_loci.size(), j=0; i < iEnd; ++i)
			for(UINT p=0; p < pEnd; ++p)
				m_alleles[j++] = ind->allele(m_loci[i], p);

		double resDouble;
		PyCallFunc(m_func, "(O)", m_numArray, resDouble, PyObj_As_Double);
		return resDouble;
	}

}
