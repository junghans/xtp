/*
 *            Copyright 2009-2012 The VOTCA Development Team
 *                       (http://www.votca.org)
 *
 *      Licensed under the Apache License, Version 2.0 (the "License")
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef VOTCA_CTP_CALCULATOR_H
#define VOTCA_CTP_CALCULATOR_H



#include <votca/tools/property.h>

namespace votca { namespace ctp {

class Calculator
{
public:

                    Calculator() {}
    virtual        ~Calculator() {}

    // reads-in default options from the shared folder
    void LoadDefaults() {

        // get the path to the shared folders with xml files
        char *votca_share = getenv("VOTCASHARE");
        if(votca_share == NULL) throw std::runtime_error("VOTCASHARE not set, cannot open help files.");
        
        std::string name = Identify();
        std::string xmlFile = std::string(getenv("VOTCASHARE")) + std::string("/ctp/xml/") + name + std::string(".xml");
        
        //cout << "Calculator " << name  << " reading from " << xmlFile << endl;
        
        // load the xml description of the calculator (with the default and test values)
        votca::tools::load_property_from_xml(_options, xmlFile);

        // override test values with the default values
        _options.ResetFromDefaults();
        
        //cout << XML << _options;

    };

    virtual std::string  Identify() = 0;
    virtual void    Initialize(votca::tools::Property *options) = 0;
    void            setnThreads(int nThreads) { _nThreads = nThreads; _maverick = (_nThreads == 1) ? true : false; }

protected:

    int _nThreads;
    bool _maverick;
    votca::tools::Property _options;

};

}}

#endif /* _QMCALCULATOR_H */