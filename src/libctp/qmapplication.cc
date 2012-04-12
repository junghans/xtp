#include <votca/ctp/qmapplication.h>
#include <votca/ctp/version.h>


namespace votca { namespace ctp {

QMApplication::QMApplication() {
    Calculatorfactory::RegisterAll();
}


void QMApplication::Initialize(void) {
    votca::csg::TrajectoryWriter::RegisterPlugins();
    votca::csg::TrajectoryReader::RegisterPlugins();

    Calculatorfactory::RegisterAll();

    namespace propt = boost::program_options;

    AddProgramOptions() ("options,o", propt::value<string>(),
                         "  calculator options");
    AddProgramOptions() ("file,f", propt::value<string>(),
                         "  state file");
    AddProgramOptions() ("first-frame,i", propt::value<int>()->default_value(1),
                         "  start from this frame");
    AddProgramOptions() ("nframes,n", propt::value<int>()->default_value(-1),
                         "  number of frames to process");
    AddProgramOptions() ("nthreads,t", propt::value<int>()->default_value(1),
                         "  number of threads to create");
}


bool QMApplication::EvaluateOptions(void) {
    CheckRequired("options", "Please provide an xml file with calculator options");
    CheckRequired("file", "Please provide the state file");
    return true;
}


void QMApplication::Run() {

    load_property_from_xml(_options, _op_vm["options"].as<string>());

    int nThreads = OptionsMap()["nthreads"].as<int>();
    int nframes = OptionsMap()["nframes"].as<int>();
    int fframe = OptionsMap()["first-frame"].as<int>();
    if (fframe-- == 0) throw runtime_error("ERROR: First frame is 0, counting "
                                           "in VOTCA::CTP starts from 1.");

    cout << "Initializing calculators " << endl;
    BeginEvaluate(nThreads);



    string statefile = OptionsMap()["file"].as<string>();
    StateSaverSQLite statsav;
    statsav.Open(_top, statefile);

    while (statsav.NextFrame()) {
        cout << "Evaluating frame " << _top.getDatabaseId() << endl;
        EvaluateFrame();
        statsav.WriteFrame();
    }
    statsav.Close();
    EndEvaluate();

}




void QMApplication::AddCalculator(QMCalculator* calculator) {
    _calculators.push_back(calculator);
}


void QMApplication::BeginEvaluate(int nThreads = 1) {
    list< QMCalculator* > ::iterator it;
    for (it = _calculators.begin(); it != _calculators.end(); it++) {
        cout << "... " << (*it)->Identify() << " ";
        (*it)->setnThreads(nThreads);
        (*it)->Initialize(&_top, &_options);        
        cout << endl;
    }
}

bool QMApplication::EvaluateFrame() {
    list< QMCalculator* > ::iterator it;
    for (it = _calculators.begin(); it != _calculators.end(); it++) {
        cout << "... " << (*it)->Identify() << " ";
        (*it)->EvaluateFrame(&_top);
        cout << endl;
    }
}

void QMApplication::EndEvaluate() {
    list< QMCalculator* > ::iterator it;
    for (it = _calculators.begin(); it != _calculators.end(); it++) {
        (*it)->EndEvaluate(&_top);
    }
}

void QMApplication::ShowHelpText(std::ostream &out) {
    string name = ProgramName();
    if (VersionString() != "") name = name + ", version " + VersionString();
    votca::ctp::HelpTextHeader(name);
    HelpText(out);
    out << "\n\n" << OptionsDesc() << endl;
}



}}
