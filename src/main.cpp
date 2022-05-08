#include "common.h"
#include "domain.h"
#include "instance.h"
#include "parser.h"
#include "generalized_planning_problem.h"
#include "program.h"
#include "node.h"
#include "generalized_domain.h"
#include "landmark_graph.h"
#include "heuristic.h"
#include "engine.h"

int main(  int argc, const char* argv[] ){
	if( argc < 4 ){
		cout << "[ERROR] Incorrect input." << endl;
		cout << "[INFO] Execute: ./main.bin <BFS|PGP> <program_lines> <problem_folder/> <list of heuristic names>" << endl;
		cout << "[INFO] Example: ./main.bin PGP 5 domains/synthesis/visitall/ landmarks h6" << endl;
		cout << "[INFO] List of heuristics is optional, the domain file must be domain.txt with all instances numbered from 1.txt to [n].txt" << endl;
		return -1;
	}

	int precision = 3;
	cout.setf( ios::fixed );
	cout.precision( precision );

	time_t start, parse_time;
	time( &start );

    string search_mode = string(argv[1]); // BFS or PGP
    if(search_mode != "BFS" and search_mode != "PGP" ){
        cout << "[ERROR] Search mode is wrong, either use BFS or PGP.\n";
        cout << "[INFO] Execute: ./main.bin <BFS|PGP> <program_lines> <problem_folder/> <list of heuristic names>" << endl;
        cout << "[INFO] Example: ./main.bin PGP 5 domains/synthesis/visitall/ landmarks h6" << endl;
        return -10;
    }
    bool is_progressive = (search_mode=="PGP"); // BFS => progressive = false; PGP => progressive = true;

	int program_lines = stoi( argv[2] );
	
	string problem_folder = string( argv[3] );

    vector< string > heuristic_names( argc - 4 );
    string heuristics;
    bool preprocess_landmarks = false;
    for( int i = 4; i < argc; i++ ) {
        heuristic_names[ i-4 ] = string(argv[i]);
        heuristics += "_" + heuristic_names[i-4];
        if( heuristic_names[ i-4 ] == "landmarks" or heuristic_names[i-4] == "normlandmarks")
            preprocess_landmarks = true;
    }
    if( argc == 4 ){ // default heuristic
        heuristic_names.emplace_back("landmarks" );
        preprocess_landmarks = true;
        heuristics = "_landmarks";
    }
	
	ifstream ifs_domain( problem_folder + "domain.txt" );
	if( !ifs_domain ){
		cout << "[ERROR] domain.txt does not exist." << endl;
		return -2;
	}
	ifs_domain.close();
	
	auto *parser = new Parser();
	auto *dom = new Domain();
		
	if( !parser->parseDomain( dom, problem_folder + "domain.txt" ) ){
		cout << "[ERROR] while parsing domain.txt" << endl;
		delete dom;
		delete parser;
		return -3;
	}

    size_t problem_pos = problem_folder.find_first_of( '/' );
    string outfile = "experiments/" + dom->getName() + "_" + search_mode + "_" + to_string(program_lines) + heuristics;
    if( problem_pos != string::npos ){
        outfile = "experiments/" + problem_folder.substr( problem_pos + 1, problem_folder.length() - 2 - problem_pos )
                + "_" + search_mode + "_" + to_string( program_lines ) + heuristics;

    }

	time( &parse_time );
	cout << "[INFO] Parsed domain. [" << difftime( parse_time, start ) << "]" << endl;
	
	// Generating the GP problem
	auto *gpp = new GeneralizedPlanningProblem();
	gpp->addDomain( dom );
    gpp->setProgressive( is_progressive );
	
	int i = 1;	
	while( true ){
		auto *ins = new Instance();
		string input_instance = problem_folder + to_string( i ) + ".txt";
		i++;
		
		ifstream ifs_instance( input_instance );
		if( !ifs_instance ){
			delete ins;
			break;
		}
		ifs_instance.close();
		
		if( parser->parseInstance( dom, ins, input_instance, i-2 ) )
			gpp->addInstance( ins );
		else{
			cout << "[ERROR] while parsing " << input_instance << endl;
            delete ins;
            delete gpp;
            delete parser;
			return -4;
		}
	}

	// After reading all instances, update the bounds for each predicate in every instance
	gpp->updatePredicateBounds();

	time_t gpp_time;
	time( &gpp_time );
	cout << "[INFO] Generalized Planning Problem created. [" << difftime( gpp_time, parse_time ) << "]" << endl;
	
	auto *gd = new GeneralizedDomain( dom, program_lines );

	time_t gd_time;
	time( &gd_time );
	cout << "[INFO] Generalized Domain created. [" << difftime( gd_time, gpp_time ) << "]" << endl;

	vector<LandmarkGraph*> landmark_graphs;
    if( preprocess_landmarks ) {
        for (int id = 0; id < gpp->getNumInstances(); id++) {
            landmark_graphs.push_back(new LandmarkGraph(dom, gpp->getInstance(id)));
            ofstream ofs_prog(outfile + "-" + to_string(id) + ".lg" );
            ofs_prog << landmark_graphs[id]->toString() << endl;
            ofs_prog.close();
        }
    }

    time_t landmarks_time;
    time( &landmarks_time );
    cout << "[INFO] Landmarks created. [" << difftime( landmarks_time, gd_time ) << "]" << endl;

	auto *engine = new BFS( program_lines, gd, gpp );
    if( preprocess_landmarks ) {
        engine->setLandmarks(landmark_graphs);
    }

	time_t engine_time;
	time( &engine_time );
	cout << "[INFO] Engine created. [" << difftime( engine_time, landmarks_time ) << "]" << endl;

    for( const auto& h_name : heuristic_names ){
        if( h_name == "h1" ) engine->addHeuristic( new H1() );
        else if( h_name == "h2" ) engine->addHeuristic( new H2() );
        else if( h_name == "h3" ) engine->addHeuristic( new H3() );
        else if( h_name == "h4" ) engine->addHeuristic( new H4() );
        else if( h_name == "h5" ) engine->addHeuristic( new H5() );
        else if( h_name == "h6" ) engine->addHeuristic( new H6() );
        else if( h_name == "h7" ) engine->addHeuristic( new H7() );
        else if( h_name == "h8" ) engine->addHeuristic( new H8() );
        else if( h_name == "h9" ) engine->addHeuristic( new H9() );
        else if( h_name == "landmarks" ) {
            assert( preprocess_landmarks );
            engine->addHeuristic(new LandmarksHeuristic() );
        }
        else if( h_name == "normlandmarks" ){
            assert( preprocess_landmarks );
            engine->addHeuristic(new NormalizedLandmarksHeuristic() );
        }
        else{
            delete engine;
            delete parser;
            cout << "[ERROR] heuristic " << h_name << " is unknown." << endl;
            return -5;
        }
        heuristics += "_" + h_name;
    }

	time_t init_search_time;
	time( &init_search_time );
	cout << "[INFO] Searching... [" << difftime(init_search_time,engine_time) << "]" << endl;

	Node *res = engine->solve(is_progressive);

	time_t end_time;
	time( &end_time );
    ostringstream total_time, search_time;
    total_time.setf(ios::fixed); search_time.setf(ios::fixed);
    total_time.precision( precision ); search_time.precision( precision);
    total_time << difftime(end_time, start); search_time << difftime(end_time, init_search_time );

	string output;
	
	if( res != nullptr ){
		Program *p = res->getProgram();
		
		ofstream ofs_prog( outfile + ".prog" );
		ofs_prog << p->toString( false );
		ofs_prog.close();
		
		output += "[INFO] SOLUTION FOUND!!!\n";
        output += "[INFO] Expanded nodes: " + to_string( engine->getExpanded() ) + "\n";
        output += "[INFO] Evaluated nodes: " + to_string( engine->getEvaluated() ) + " (deadends: " + to_string(engine->getDeadends()) + ")\n";
        output += "[INFO] Evaluated states: " + to_string( engine->getEvaluatedStates() ) + "\n";
		output += p->toString( false )  + "\n";
		output += "[INFO] Total time: " + total_time.str() + "\n";
        output += "[INFO] Search time: " + search_time.str() + "\n";
	
		cout << output;	
		cout << "[INFO] Program file: " << outfile << ".prog" << endl;	
	}	
	else{
		output += "[INFO] SOLUTION NOT FOUND :(\n";
		output += "[INFO] Expanded nodes: " + to_string( engine->getExpanded() ) + "\n";
		output += "[INFO] Evaluated nodes: " + to_string( engine->getEvaluated() ) + " (deadends: " + to_string(engine->getDeadends()) + ")\n";
        output += "[INFO] Evaluated states: " + to_string( engine->getEvaluatedStates() ) + "\n";
        output += "[INFO] Total time: " + total_time.str() + "\n";
        output += "[INFO] Search time: " + search_time.str() + "\n";
		cout << output;
	}

	ofstream ofs( outfile + ".out" );
	ofs << output;
	ofs.close();
	
	cout << "[INFO] Experiments file: " << outfile << ".out" << endl;	
	
	delete engine;
	delete parser;
}
