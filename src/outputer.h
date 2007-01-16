/***************************************************************************
 *   Copyright (C) 2004 by Bo Peng                                         *
 *   bpeng@rice.edu
 *                                                                         *
 *   $LastChangedDate$
 *   $Rev$                                                    *
 *                                                                         *
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

#ifndef _OUTPUTER_H
#define _OUTPUTER_H
/**
\file
\brief head file of class outputer: public Operator
*/
#include "utility.h"
#include "operator.h"
#include <iostream>

#include <iomanip>
using std::setw;
using std::hex;
using std::dec;

namespace simuPOP
{
	/**
	\brief outputer is a (special) subclass of Operator that will output files with
	different format.

	@author Bo Peng
	*/

	class outputer: public Operator
	{

		public:
			/// constructor. default to be always active.
			outputer(string output=">", string outputExpr="",
				int stage=PostMating, int begin=0, int end=-1, int step=1, vectorl at=vectorl(),
				int rep=REP_ALL, int grp=GRP_ALL, const vectorstr& infoFields=vectorstr()):
			Operator(output, outputExpr, stage, begin, end, step, at, rep, grp, infoFields)
			{
			};

			/// destructor
			virtual ~outputer(){};

			virtual Operator* clone() const
			{
				return new outputer(*this);
			}

	};

	class outputHelper: public outputer
	{

		public:
			///
			outputHelper(string str="\n", string output=">", string outputExpr="",
				int stage=PostMating, int begin=0, int end=-1, int step=1, vectorl at=vectorl(),
				int rep=REP_ALL, int grp=GRP_ALL, const vectorstr& infoFields=vectorstr()):
			outputer( output, outputExpr, stage, begin, end,
				step, at, rep, grp, infoFields), m_string(str)
			{
			}

			/// simply output some info
			virtual bool apply(population& pop)
			{
				ostream& out = this->getOstream(pop.dict());
				out << m_string;
				this->closeOstream();
				return true;
			}

			///// destructor
			virtual ~outputHelper()
			{
			}

			virtual Operator* clone() const
			{
				return new outputHelper(*this);
			}

			/// set output string.
			void setString(const string str)
			{
				m_string = str;
			}

			virtual string __repr__()
			{
				string reprStr;
				for(size_t i=0; i<10 && i < m_string.size(); ++i)
					if(m_string[i] != '\n')
						reprStr += m_string[i];
				if( m_string.size() > 10)
					reprStr += "... ";
				return "<simuPOP::output " + reprStr + "> " ;
			}

		private:
			string m_string;
	};

	/// dump the content of a population.

	class dumper: public outputer
	{
		public:
			/** \brief dump population

			\param alleleOnly only display allele
			\param infoOnly only display info
			\param dispWidth width of allele display, default to 1
			\param ancestralPops whether or not display ancestral populations, default to False
			\param chrom chromsoome(s) to display
			\param loci loci to display
			\param subPop only display subPop(s)
			\param indRange range(s) of individuals to display
			\param max max number of individuals to display, default to 100.
			This is to avoid careless dump of huge populations.
			\param output output file, default to standard output.
			\param outputExpr and other parameters: refer to help(baseOperator.__init__)

			*/
			dumper( bool alleleOnly=false, bool infoOnly=false, bool ancestralPops=false, int dispWidth=1, UINT max=100,
				const vectori& chrom=vectori(), const vectori& loci=vectori(), const vectoru& subPop=vectoru(),
				const vectorlu& indRange=vectorlu(),
				string output=">", string outputExpr="",
				int stage=PostMating, int begin=0, int end=-1, int step=1, vectorl at=vectorl(),
				int rep=REP_ALL, int grp=GRP_ALL, const vectorstr& infoFields=vectorstr()):
			outputer(output, outputExpr, stage, begin, end, step, at, rep, grp, infoFields),
				m_alleleOnly(alleleOnly), m_infoOnly(infoOnly), m_dispAncestry(ancestralPops), m_width(dispWidth),
				m_chrom(chrom), m_loci(loci), m_subPop(subPop), m_indRange(indRange), m_max(max)
				{}

			virtual Operator* clone() const
			{
				return new dumper(*this);
			}

			/// only show alleles (not structure, gene information?
			bool alleleOnly()
			{
				return m_alleleOnly;
			}

			///
			void setAlleleOnly(bool alleleOnly)
			{
				m_alleleOnly = alleleOnly;
			}

			/// only show info
			bool infoOnly()
			{
				return m_infoOnly;
			}

			///
			void setInfoOnly(bool infoOnly)
			{
				m_infoOnly = infoOnly;
			}

			virtual bool apply(population& pop)
			{
					ostream& out = this->getOstream(pop.dict());

					/// dump population structure
					if(! alleleOnly() )
					{
						out << "Ploidy:         \t" << pop.ploidy() << endl;
						out << "Number of chrom:\t" << pop.numChrom() << endl;
						out << "Number of loci: \t";
						for(UINT i=0; i < pop.numChrom(); ++i)
							out << pop.numLoci(i) << " ";
						out << endl;
						out << "Maximum allele state:\t" << pop.maxAllele() << endl;
						out << "Loci positions: " << endl;
						for(UINT ch=0; ch < pop.numChrom(); ++ch)
						{
							cout << "\t\t";
							for(UINT i=0; i < pop.numLoci(ch); ++i)
								out << pop.locusPos( pop.absLocusIndex(ch,i) ) << " ";
							out << endl;
						}
						out << "Loci names: " << endl;
						for(UINT ch=0; ch < pop.numChrom(); ++ch)
						{
							out << "\t\t";
							for(UINT i=0; i < pop.numLoci(ch); ++i)
								out << pop.locusName( pop.absLocusIndex(ch,i) ) << " ";
							out << endl;
						}
						out << "population size:\t" << pop.popSize() << endl;
						out << "Number of subPop:\t" << pop.numSubPop() << endl;
						out << "Subpop sizes:   \t";
						for(UINT i=0, iEnd = pop.numSubPop(); i < iEnd;  ++i)
							out << pop.subPopSize(i) << " ";
						out << endl;
						out << "Number of ancestral populations:\t" << pop.ancestralDepth() << endl;
					}

					if(! m_infoOnly)
					{
						/// dump all genotypic info
						if(pop.maxAllele() >= 10 && pop.maxAllele() < 100)
							m_width = 2;
						else if( pop.maxAllele() >=100)
							m_width = 3;

						// get individual ranges from subpop
						vectorlu range = m_indRange;
						if(m_indRange.empty())
						{
							if( m_subPop.empty() )// all subpop
							{
								for(UINT sp=0; sp < pop.numSubPop();  sp++)
								{
									if( pop.subPopSize(sp) == 0)
										continue;
									range.push_back( pop.subPopBegin(sp));
									range.push_back( pop.subPopEnd(sp));
								}
							}
							else
							{
								for(vectoru::iterator sp=m_subPop.begin();  sp != m_subPop.end();  sp++)
								{
									if( pop.subPopSize(*sp) == 0)
										continue;
									range.push_back( pop.subPopBegin(*sp));
									range.push_back( pop.subPopEnd(*sp));
								}
							}
						}
						out << "individual info: " << endl;
						UINT count = 0;
						for(size_t i=0; i < range.size(); i+=2)
						{
							UINT sp = pop.subPopIndPair(range[i]).first;
							out << "sub population " << sp << ":" << endl;

							for( population::IndIterator ind = pop.indBegin()+range[i]; ind != pop.indBegin()+range[i+1]; ++ind)
							{
								out << setw(4) << count++ << ": ";
								ind->display(out, m_width, m_chrom, m_loci);
								out << endl;

								if(m_max > 0 && count > m_max && count < pop.popSize())
								{
									cout << "population size is " << pop.popSize() << " but dumper() only dump "
										<< m_max << " individuals" << endl
										<< "Use parameter max=-1 to output all individuals." << endl;
									goto done;
								}
							}
						}

						done:
						out << "End of individual info." << endl << endl;

						if(!m_dispAncestry)
						{
							if( pop.ancestralDepth() == 0)
								out << endl << "No ancenstral population recorded." << endl;
							else
								out << endl << "Ignoring " << pop.ancestralDepth() << " ancenstral population(s)." << endl;
						}
						else
						{
							for(size_t i=0; i<pop.ancestralDepth(); ++i)
							{
								pop.useAncestralPop(i+1);
								out << endl << "Ancestry population " << i+1 << endl;

								out << "population size:\t" << pop.popSize() << endl;
								out << "Number of subPop:\t" << pop.numSubPop() << endl;
								out << "Subpop sizes:   \t";
								for(UINT i=0, iEnd = pop.numSubPop(); i < iEnd;  ++i)
									out << pop.subPopSize(i) << " ";
								out << endl;

								out << "individual info: " << endl;

								// get individual ranges from subpop
								vectorlu range = m_indRange;
								if(m_indRange.empty())
								{
												  // all subpop
									if( m_subPop.empty() )
									{
										for(UINT sp=0; sp < pop.numSubPop();  sp++)
										{
											if( pop.subPopSize(sp) == 0)
												continue;
											range.push_back( pop.subPopBegin(sp));
											range.push_back( pop.subPopEnd(sp));
										}
									}
									else
									{
										for(vectoru::iterator sp=m_subPop.begin();  sp != m_subPop.end();  sp++)
										{
											if( pop.subPopSize(*sp) == 0)
												continue;
											range.push_back( pop.subPopBegin(*sp));
											range.push_back( pop.subPopEnd(*sp));
										}
									}
								}
								out << "individual info: " << endl;
								UINT count = 0;
								for(size_t j=0; j < range.size(); j+=2)
								{
									UINT sp = pop.subPopIndPair(range[j]).first;
									out << "sub population " << sp << ":" << endl;

									for( population::IndIterator ind = pop.indBegin()+range[j]; ind != pop.indBegin()+range[j+1]; ++ind)
									{
										out << setw(4) << count++ << ": " ;
										ind->display(out, m_width, m_chrom, m_loci);
										out << endl;

										if(m_max > 0 && count > m_max && count < pop.popSize())
										{
											cout << "population size is " << pop.popSize() << " but dumper() only dump "
												<< m_max << " individuals" << endl
												<< "Use parameter max=-1 to output all individuals." << endl;
											goto doneAnces;
										}
									}
								}

								doneAnces:
								out << "End of individual info." << endl << endl;
							}					  // next ancestry
							// IMPORTANT. Reset ancestral pop
							pop.useAncestralPop(0);
						}						  // dispAncestry
					}
					this->closeOstream();
				return true;
			}

			virtual ~dumper(){};

			virtual string __repr__()
			{
				return "<simuPOP::dumper>" ;
			}

		private:
			/// only output alleles, not structure info
			bool m_alleleOnly;

			/// only display info
			bool m_infoOnly;

			/// whether or not display ancestral populations
			bool m_dispAncestry;

			/// disp width when outputing alleles
			int m_width;

			///
			vectori m_chrom;

			///
			vectori m_loci;

			///
			vectoru m_subPop;

			///
			vectorlu m_indRange;

			/// only output first ... individuals. Good for large population
			UINT m_max;
	};

	/// save population to a file

	class savePopulation: public outputer
	{
		public:
			savePopulation( string output="", string outputExpr="",
				string format = "bin", bool compress=true, int stage=PostMating, int begin=0, int end=-1,
				int step=1, vectorl at=vectorl(), int rep=REP_ALL, int grp=GRP_ALL, const vectorstr& infoFields=vectorstr()):
			outputer( "", "", stage, begin, end, step, at, rep, grp, infoFields),
				m_filename(output), m_filenameParser(outputExpr), m_format(format), m_compress(compress)
			{
				if(output == "" && outputExpr == "")
					throw ValueError("Please specify one of output and outputExpr.");
			}

			~savePopulation()
			{
			}

			virtual Operator* clone() const
			{
				return new savePopulation(*this);
			}

			virtual bool apply(population& pop)
			{
				string filename;
				if( m_filename != "")
					filename = m_filename;
				else
				{
					m_filenameParser.setLocalDict(pop.dict());
					filename = m_filenameParser.valueAsString();
				}
				DBG_DO(DBG_OUTPUTER, cout << "Save to file " << filename << endl);
				pop.savePopulation(filename, m_format, m_compress);
				return true;
			}

			virtual string __repr__()
			{
				return "<simuPOP::save population>" ;
			}

		private:
			/// filename,
			string m_filename;

			/// or an expression that will be evaluated dynamically
			Expression m_filenameParser;

			/// can specify format, default to 'auto'
			string m_format;

			/// whether or not compress population
			bool m_compress;
	};

}
#endif
