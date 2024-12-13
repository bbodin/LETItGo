/*
 * GeneratorTest.cpp
 *
 */

#define BOOST_TEST_MODULE GenerateSampleTest
#include <boost/test/unit_test.hpp>
#include <letitgo.h>

#include <sstream>
#include <string>
#include <vector>

std::string generateLatexSchedule(const LETModel &model, TIME_UNIT hyperPeriod) {
    std::ostringstream latex;
    latex << "\\begin{figure}\n\n";
    latex << model.getTikzSchedule(hyperPeriod);
    latex << "\n\n"
          << "\\caption{Example of a LET application with " << model.getTaskCount()
          << " tasks, and " << model.getDependencyCount() << " communication channels.}\n"
          << "\\label{fig:example_execution}\n"
          << "\\end{figure}";
    return latex.str();
}


std::string generateLatexFigure(const LETModel &model) {
    std::ostringstream latex;
    latex << "\\begin{figure}\n"
          << "\\centering\n"
          << "\\begin{multicols}{2}\n"
          << "%L'arbre\n"
          << "\\centering\n";
    latex << model.getTikzDAG();
    latex << "\\columnbreak\n\n"
          << "\\begin{center}\n";
    latex << model.getTabular();
    latex << "\\end{center}\n"
          << "\\end{multicols}\n"
          << "\\caption{An instance of $" << model.getTaskCount() << "$ periodic tasks and the associated DAG ${\\cal G}$.}\n"
          << "\\label{fig:example_dag}\n"
          << "\\end{figure}\n";
    return latex.str();
}


std::string generateLatexText(const LETModel &model, DEPENDENCY_ID d) {
    std::ostringstream latex;
    // Retrieve dependency and tasks
    Dependency dep = model.dependencies().at(d);
    Task src = model.getTaskById(dep.getFirst());
    Task dest = model.getTaskById(dep.getSecond());

    // Extract timing parameters
    TIME_UNIT srcT = src.getT();
    TIME_UNIT destT = dest.getT();
    TIME_UNIT srcR = src.getr();
    TIME_UNIT destR = dest.getr();
    TIME_UNIT destD = dest.getD();
    TIME_UNIT srcD = src.getD();

    // Compute gcd_T^e
    TIME_UNIT gcdTe = std::gcd(static_cast<long>(srcT), static_cast<long>(destT));

    // Compute M^e
    TIME_UNIT Me = gcdTe + std::ceil(static_cast<double>( srcR - destR + srcD) / gcdTe) * gcdTe;


    std::string first_set;
    std::string second_set;

    // Compute specific examples for ν_1 = 1 and ν_1 = 2
    for (int nu1 = 1; nu1 <= 2; ++nu1) {
        std::vector<int> nu2Values;
        for (int nu2 = 1; nu2 <= 50; ++nu2) { // Arbitrary limit for nu2 search
            TIME_UNIT condition = Me + srcT * nu1 - destT * nu2;
            if (condition > 0 && condition <= srcT) {
                nu2Values.push_back(nu2);
            }
        }
        std::string lset;
        for (size_t i = 0; i < nu2Values.size(); ++i) {
            if (i > 0) lset += ",";
            lset += std::to_string(nu2Values[i]);
        }
        if (nu1 == 1) {
            first_set = lset;
        } else {
            second_set = lset;
        }
    }


    latex << std::endl;
    latex << "% paragraph intro";
    latex << std::endl;

    // Ensure there is at least one task in the model
    if (model.getTaskCount() > 0) {
        const Task &task = model.tasks().front(); // Get the first task
        TASK_ID id = task.getId();
        TIME_UNIT r = task.getr();
        TIME_UNIT D = task.getD();
        INTEGER_TIME_UNIT T = task.getT();

        latex << "For example, Figure \\ref{fig:example_dag} depicts a LET application with "
              << model.getTaskCount() << " LET tasks. ";
        latex << "The first task $t_{" << id << "}$ has $(r_{" << id << "} = " << r
              << ", D_{" << id << "} = " << D << ", T_{" << id << "} = " << T << ")$. ";
        latex << "This means, the first execution of $t_{" << id << "}$ denoted $(t_{" << id << "}, 1)$ starts at time "
              << r << " and must finish at time " << (r + D) << ". ";
        latex << "Its second execution starts at ${\\cal S}(t_{" << id << "}, 2) = " << r << " + (2-1)\\times " << T
              << " = " << (r + T) << "$.";
    } else {
        latex << "No tasks available in the LETModel.";
    }


    latex << std::endl;
    latex << "% paragraph 1";
    latex << std::endl;
    latex << "For example, the first data dependencies induced by the arc $e=(t_{" << dep.getFirst()
          << "},t_{" << dep.getSecond() << "})\\in E$ are"
          << " $\\Exec{t_{" << dep.getFirst()
          << "}}{1}\\rightarrow \\Exec{t_{" << dep.getSecond() << "}}{\\nu_{" << dep.getSecond() << "}}$ "
          << " with $\\nu_{" << dep.getSecond() << "} \\in\\{" << first_set << "\\}$,"
          << " $\\Exec{t_{" << dep.getFirst()
          << "}}{2}\\rightarrow \\Exec{t_{" << dep.getSecond() << "}}{\\nu_{" << dep.getSecond() << "}}$ "
          << " with $\\nu_{" << dep.getSecond() << "} \\in\\{" << second_set << "\\}$,"

          << " etc.." << std::endl;



    latex << std::endl;
    latex << "% paragraph 2";
    latex << std::endl;

    // Write LaTeX introduction
    latex << "For example, let us consider again the dependence $e=(t_{" << dep.getFirst()
          << "},t_{" << dep.getSecond() << "})\\in E$ between tasks $t_{" << dep.getFirst()
          << "}$ and $t_{" << dep.getSecond() << "}$ from the Figure~\\ref{fig:example_dag}.\n";
    latex << "We get $gcd_T^{e}=\\gcd(" << srcT << "," << destT << ")=" << gcdTe << "$ and \n";
    latex << "$M^e=" << gcdTe << "+\\lceil \\frac{" << destR << "-" << srcR << "+" << destD
          << "}{" << gcdTe << "} \\rceil \\times " << gcdTe << "=" << Me << "$.\n";

    // Write condition for dependency
    latex << "Then for any couple $(\\nu_{" << dep.getFirst() << "}, \\nu_{" << dep.getSecond()
          << "})\\in \\mathbb{N}^\\star\\times\\mathbb{N}^\\star$,  \n";
    latex << "$\\Exec{t_{" << dep.getFirst() << "}}{\\nu_{" << dep.getFirst() << "}}"
          << "\\rightarrow\\Exec{t_{" << dep.getSecond() << "}}{\\nu_{" << dep.getSecond() << "}}$"
          << " if and only if\n";
    latex << "$" << srcT << " \\geq " << Me << "+" << srcT << "\\times \\nu_{" << dep.getFirst()
          << "} - " << destT << "\\times \\nu_{" << dep.getSecond() << "} >0$.\n";

    // Dependencies for specific executions
    latex << "These dependencies between the first executions of $t_{" << dep.getFirst()
          << "}$ and $t_{" << dep.getSecond()
          << "}$ are depicted on Figure \\ref{fig:example_execution}.\n";


        latex << "For example, setting $\\nu_{" << dep.getFirst() << "}=1$, we obtain ";

    latex << "$\\nu_{" << dep.getSecond() << "}\\in\\{";
    latex << first_set;
    latex << "\\}$";
    latex << " associated to the dependencies $\\Exec{t_{" << dep.getFirst() << "}}{1}\\rightarrow \\Exec{t_{"
    << dep.getSecond() << "}}{\\nu_{" << dep.getSecond()
          << "}}$.";
    latex << "Similarly, for $\\nu_{" << dep.getFirst() << "}=2$, we get ";
    latex << "$\\nu_{" << dep.getSecond() << "}\\in\\{";
    latex << second_set;
    latex << "\\}$";
    latex << ".";




    return latex.str();
}


std::string generateDependencyLatency(const LETModel &model, DEPENDENCY_ID d, std::vector<std::pair<int, int>> tuples ) {
    std::ostringstream latex;

    // Retrieve dependency and associated tasks
    Dependency dep = model.dependencies().at(d);
    Task src = model.getTaskById(dep.getFirst());
    Task dest = model.getTaskById(dep.getSecond());

    // Extract timing properties
    TIME_UNIT Ti = src.getT();    // Source task period
    TIME_UNIT Tj = dest.getT(); // Destination task period
    TIME_UNIT Ri = src.getr();   // Source task release time
    TIME_UNIT Rj = dest.getr(); // Destination task release time

    // Generate introductory sentence
    latex << "Let us consider again the arc $e=(t_{" << dep.getFirst() << "},t_{" << dep.getSecond()
          << "})$ for our example given by Figure \\ref{fig:example_dag}. ";


    latex << "As seen before, the tuples ";
    for (size_t i = 0; i < tuples.size(); ++i) {
        if (i > 0) {
            latex << " and ";
        }
        latex << "$(" << tuples[i].first << "," << tuples[i].second << ")$";
    }
    latex << " are belonging to ${\\cal R}(e)$, we get\n";

    // Calculate latencies for each tuple
    for (const auto &tuple : tuples) {
        int Nui = tuple.first;
        int Nuj = tuple.second;

        // Calculate latency
        TIME_UNIT latency = Rj - Ri + Ti - Tj - (Ti*Nui - Tj * Nuj);

        latex << "${\\cal L}_{" << Nui << "," << Nuj << "}(e)="
              << Rj << "-" << Ri << "+" << Ti << "-" << Tj
              << "-(" << Ti << "\\times " << Nui << "-" << Tj << "\\times " << Nuj << ")="
              << latency << "$";
        if (&tuple != &tuples.back()) {
            latex << " and ";
        } else {
            latex << ".";
        }
    }

    return latex.str();
}

INTEGER_TIME_UNIT compute_alphae_ai_aj (const LETModel &model,const Dependency &d, EXECUTION_COUNT ai, EXECUTION_COUNT aj) {
    TASK_ID ti_id = d.getFirst();
    TASK_ID tj_id = d.getSecond();

    Task ti = model.getTaskById(ti_id);
    Task tj = model.getTaskById(tj_id);

    INTEGER_TIME_UNIT Ti = ti.getT();
    INTEGER_TIME_UNIT Tj = tj.getT();
    INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);

    return  (Ti * ai - Tj * aj) / gcdeT;
}
INTEGER_TIME_UNIT compute_pi_min (const LETModel &model,const Dependency &d, EXECUTION_COUNT ai, EXECUTION_COUNT aj, EXECUTION_COUNT Ki, EXECUTION_COUNT Kj) {
    TASK_ID ti_id = d.getFirst();
    TASK_ID tj_id = d.getSecond();

    Task ti = model.getTaskById(ti_id);
    Task tj = model.getTaskById(tj_id);

    INTEGER_TIME_UNIT Ti = ti.getT();
    INTEGER_TIME_UNIT Tj = tj.getT();
    INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);

    auto Di = ti.getD();
    //auto Dj = tj.getD();

    auto ri = ti.getr();
    auto rj = tj.getr();

    auto gcdK = std::gcd(Ti * Ki, Tj * Kj);
    auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;
    INTEGER_TIME_UNIT alphae_ai_aj = compute_alphae_ai_aj (model,d, ai, aj);
    return std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdK);
}
INTEGER_TIME_UNIT compute_pi_max (const LETModel &model,const Dependency &d, EXECUTION_COUNT ai, EXECUTION_COUNT aj, EXECUTION_COUNT Ki, EXECUTION_COUNT Kj) {
    TASK_ID ti_id = d.getFirst();
    TASK_ID tj_id = d.getSecond();

    Task ti = model.getTaskById(ti_id);
    Task tj = model.getTaskById(tj_id);

    INTEGER_TIME_UNIT Ti = ti.getT();
    INTEGER_TIME_UNIT Tj = tj.getT();
    INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);

    auto Di = ti.getD();
    //auto Dj = tj.getD();

    auto ri = ti.getr();
    auto rj = tj.getr();

    auto gcdK = std::gcd(Ti * Ki, Tj * Kj);
    auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;
    INTEGER_TIME_UNIT alphae_ai_aj = compute_alphae_ai_aj (model,d, ai, aj);
    return std::floor((-Me + Ti - alphae_ai_aj * gcdeT) / gcdK);
}

std::string generateAlpha(const LETModel &model, DEPENDENCY_ID edgeId, EXECUTION_COUNT Ki, EXECUTION_COUNT Kj) {
    std::ostringstream latex;

    // Retrieve dependency
    Dependency dep = model.dependencies().at(edgeId);
    TASK_ID i = dep.getFirst();  // Source task ID
    TASK_ID j = dep.getSecond(); // Destination task ID

    // Define ranges for a_i and a_j based on Ki and Kj
    std::vector<EXECUTION_COUNT> a_i(Ki);
    std::vector<EXECUTION_COUNT> a_j(Kj);

    // Fill the vectors with values 1 through Ki and 1 through Kj
    for (EXECUTION_COUNT k = 1; k <= Ki; ++k) {
        a_i[k - 1] = k;
    }
    for (EXECUTION_COUNT k = 1; k <= Kj; ++k) {
        a_j[k - 1] = k;
    }


    // Start the description
    latex << "For example, let us consider again the LET communication $e=(t_{" << i << "},t_{" << j << "})$ "
          << "from the instance presented by Figure \\ref{fig:example_dag}.\n";
    latex << "Let us also suppose here that $K_{" << i << "}=" << Ki << "$ and $K_{" << j << "}=" << Kj << "$.\n";
    latex << "Figure \\ref{fig:alphaetpiminmax} presents the associated functions "
          << "$\\alpha_e(a_{" << i << "},a_{" << j << "})$, $\\pi^{\\text{max}}_e(a_{" << i << "},a_{" << j << "})$, "
          << "and $\\pi^{\\text{min}}_e(a_{" << i << "},a_{" << j << "})$ for "
          << "$(a_{" << i << "},a_{" << j << "})\\in\\{1,\\ldots," << Ki << "\\}\\times\\{1,\\ldots," << Kj << "\\}$.\n";



    // Begin LaTeX figure
    latex << "\\begin{figure}\n"
          << "\\centering\n";

    // Generate table for alpha_e(a_i, a_j)
    latex << "\\begin{center}\n"
          << "\\begin{tabular}{|c";
    for (size_t col = 0; col < a_j.size(); ++col) latex << "|c";
    latex << "|}\n\\hline\n"
          << "\\diagbox{$a_{" << i << "}$}{$a_{" << j << "}$}";
    for (EXECUTION_COUNT aj : a_j) {
        latex << " & $" << aj << "$";
    }
    latex << " \\\\\n\\hline\n";
    for (EXECUTION_COUNT ai : a_i) {
        latex << "$" << ai << "$";
        for (EXECUTION_COUNT aj : a_j) {
            INTEGER_TIME_UNIT alpha = compute_alphae_ai_aj(model, dep, ai, aj);
            latex << " & $" << alpha << "$";
        }
        latex << " \\\\\n\\hline\n";
    }
    latex << "\\end{tabular}\n"
          << "\\vskip0.5cm\n"
          << "$\\alpha_e(a_{" << i << "},a_{" << j << "})$\n"
          << "\\end{center}\n";
    latex << "\\begin{multicols}{2}\n";
    // Generate table for pi^max_e(a_i, a_j)
    latex << "\\begin{center}\n"
          << "\\begin{tabular}{|c";
    for (size_t col = 0; col < a_j.size(); ++col) latex << "|c";
    latex << "|}\n\\hline\n"
          << "\\diagbox{$a_{" << i << "}$}{$a_{" << j << "}$}";
    for (EXECUTION_COUNT aj : a_j) {
        latex << " & $" << aj << "$";
    }
    latex << " \\\\\n\\hline\n";
    for (EXECUTION_COUNT ai : a_i) {
        latex << "$" << ai << "$";
        for (EXECUTION_COUNT aj : a_j) {
            INTEGER_TIME_UNIT piMax = compute_pi_max(model, dep, ai, aj, Ki, Kj);
            latex << " & $" << piMax << "$";
        }
        latex << " \\\\\n\\hline\n";
    }
    latex << "\\end{tabular}\n"
          << "\\vskip0.5cm\n"
          << "$\\pi^{\\text{max}}_e(a_{" << i << "},a_{" << j << "})$\n"
          << "\\end{center}\n"
          << "\\columnbreak\n";

    // Generate table for pi^min_e(a_i, a_j)
    latex << "\\begin{center}\n"
          << "\\begin{tabular}{|c";
    for (size_t col = 0; col < a_j.size(); ++col) latex << "|c";
    latex << "|}\n\\hline\n"
          << "\\diagbox{$a_{" << i << "}$}{$a_{" << j << "}$}";
    for (EXECUTION_COUNT aj : a_j) {
        latex << " & $" << aj << "$";
    }
    latex << " \\\\\n\\hline\n";
    for (EXECUTION_COUNT ai : a_i) {
        latex << "$" << ai << "$";
        for (EXECUTION_COUNT aj : a_j) {
            INTEGER_TIME_UNIT piMin = compute_pi_min(model, dep, ai, aj, Ki, Kj);
            latex << " & $" << piMin << "$";
        }
        latex << " \\\\\n\\hline\n";
    }
    latex << "\\end{tabular}\n"
          << "\\vskip0.5cm\n"
          << "$\\pi^{\\text{min}}_e(a_{" << i << "},a_{" << j << "})$\n"
          << "\\end{center}\n";

    // End LaTeX figure
    latex << "\\end{multicols}\n"
          << "\\caption{Functions $\\alpha_e(a_{" << i << "},a_{" << j << "})$, "
          << "$\\pi^{\\text{max}}_e(a_{" << i << "},a_{" << j << "})$, and "
          << "$\\pi^{\\text{min}}_e(a_{" << i << "},a_{" << j << "})$ expressed over "
          << "$(a_{" << i << "},a_{" << j << "})\\in\\{1,\\ldots," << Ki << "\\}\\times\\{1,\\ldots," << Kj << "\\}$,"
          << " for the LET communication $e = (t_" << i << ",t_" << j << ")$, with $K_{"<< i << "} = "<< Ki << "$ and $K_{"<< j << "} = "<< Kj << "$"
          << ".}\n"
          << "\\label{fig:alphaetpiminmax}\n"
          << "\\end{figure}";


    return latex.str();
}
std::string generateAlphaDescription(const LETModel &model, DEPENDENCY_ID edgeId, EXECUTION_COUNT Ki, EXECUTION_COUNT Kj) {
    std::ostringstream latex;

    // Retrieve dependency
    Dependency dep = model.dependencies().at(edgeId);
    TASK_ID i = dep.getFirst();  // Source task ID
    TASK_ID j = dep.getSecond(); // Destination task ID

    // Start the description
    latex << "For example, let us consider again the LET communication $e=(t_{" << i << "},t_{" << j << "})$ "
          << "from the instance presented by Figure \\ref{fig:example_dag}.\n";
    latex << "Let us also suppose here that $K_{" << i << "}=" << Ki << "$ and $K_{" << j << "}=" << Kj << "$.\n";
    latex << "Figure \\ref{fig:alphaetpiminmax} presents the associated functions "
          << "$\\alpha_e(a_{" << i << "},a_{" << j << "})$, $\\pi^{\\text{max}}_e(a_{" << i << "},a_{" << j << "})$, "
          << "and $\\pi^{\\text{min}}_e(a_{" << i << "},a_{" << j << "})$ for "
          << "$(a_{" << i << "},a_{" << j << "})\\in\\{1,\\ldots," << Ki << "\\}\\times\\{1,\\ldots," << Kj << "\\}$.\n";

    // Compute pi_e(p_1, p_2) = 15p_1 - 2p_2
    Task ti = model.getTaskById(i);
    Task tj = model.getTaskById(j);
    INTEGER_TIME_UNIT Ti = ti.getT();
    INTEGER_TIME_UNIT Tj = tj.getT();
    INTEGER_TIME_UNIT gcdK = std::gcd(Ti * Ki, Tj * Kj);


    // Provide an example of execution dependencies
    EXECUTION_COUNT ai = 3, aj = 1;    // Example values for a_1 and a_2
    EXECUTION_COUNT p1 = 1, p2 = 14;  // Example periods
    EXECUTION_COUNT exec_ti = ai + Ki * p1;
    EXECUTION_COUNT exec_tj = aj + Kj * p2;

    latex << "Now, we observe for example that $\\Exec{t_{" << i << "}}{" << exec_ti << "}\\rightarrow \\Exec{t_{" << j
          << "}}{" << exec_tj << "}$.\\ Indeed, $"
          << exec_ti << "=a_{" << i << "}+" << Ki << "p_{" << i << "}$ with $a_{" << i << "}=" << ai
          << "$ and $p_{" << i << "}=" << p1 << "$\\ "
          << "and $" << exec_tj << "=a_{" << j << "}+" << Kj << "p_{" << j << "}$ with $a_{" << j << "}=" << aj
          << "$ and $p_{" << j << "}=" << p2 << "$.\\ \n";

    // Compute pi_e for this example
    INTEGER_TIME_UNIT pi_e = (Ti / gcdK) * p1 - (Tj / gcdK) * p2;

    // Define the range
    std::vector<INTEGER_TIME_UNIT> pi_range;
    for (INTEGER_TIME_UNIT k = -15; k <= -11; ++k) {
        pi_range.push_back(k);
    }

    // Output range and dependency explanation
    latex << "Thus, we get $\\pi_e(p_{" << i << "},p_{" << j << "})=" << (Ti / gcdK) << "-" << (Tj / gcdK) << "\\times "
          << p2 << "=" << pi_e << "\\in\\{";
    for (size_t k = 0; k < pi_range.size(); ++k) {
        if (k > 0) latex << ",";
        latex << pi_range[k];
    }
    latex << "\\}$, and thus there exists a dependence due to the communication $e$ from \n"
          << "$\\Exec{t_{" << i << "}}{" << exec_ti << "}$ to $\\Exec{t_{" << j << "}}{" << exec_tj << "}$.";

    return latex.str();
}



BOOST_AUTO_TEST_SUITE(test_GenerateSampleTest)

BOOST_AUTO_TEST_CASE(test_generate) {

	auto sample = new LETModel();

    TASK_ID t0 = sample->addTask(1, 9, 9);
    TASK_ID t1 = sample->addTask(3, 9, 9);
    TASK_ID t2 = sample->addTask(3, 3, 8);
    TASK_ID t3 = sample->addTask(0, 1, 3);

    DEPENDENCY_ID d0 = sample->addDependency(t0, t2);
    DEPENDENCY_ID d1 = sample->addDependency(t0, t3);
    DEPENDENCY_ID d2 = sample->addDependency(t1, t2);

        BOOST_ASSERT(d0>0);
        BOOST_ASSERT(d1>0);
        BOOST_ASSERT(d2>0);

    std::cout << generateLatexFigure(*sample)  << std::endl;
    std::cout <<    generateLatexSchedule(*sample, 29) << std::endl;
}

    BOOST_AUTO_TEST_CASE(test_generate_old) {

        auto sample = new LETModel();

        TASK_ID t0 = sample->addTask(1, 4, 10);
        TASK_ID t1 = sample->addTask(1, 1, 2);
        TASK_ID t2 = sample->addTask(4, 3, 3);
        TASK_ID t3 = sample->addTask(3, 3, 10);

        DEPENDENCY_ID d0 = sample->addDependency(t0, t1);
        DEPENDENCY_ID d1 = sample->addDependency(t0, t2);
        DEPENDENCY_ID d2 = sample->addDependency(t1, t3);

        std::cout << generateLatexFigure(*sample)  << std::endl;
        std::cout <<    generateLatexSchedule(*sample, 29) << std::endl;
        std::cout << generateDependencyLatency(*sample, d0, {{1, 5}, {1, 7}} ) << std::endl;


         std::cout << std::endl;
         std::cout << std::endl;

        std::cout << generateAlpha(*sample, d0, 3, 2) << std::endl;
        std::cout << generateAlpha(*sample, d1, 3, 2) << std::endl;
        std::cout << generateAlpha(*sample, d2, 3, 2) << std::endl;

         std::cout << std::endl;
         std::cout << std::endl;
        std::cout << generateAlphaDescription(*sample, d0, 3, 2) << std::endl;

        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << sample->getXML() ;

        AgeLatencyResult res = compute_age_latency(*sample);

        std::cout <<res << std::endl;

    }


BOOST_AUTO_TEST_CASE(test_alpha) {

        auto sample = new LETModel();

        TASK_ID t0 = sample->addTask(1, 4, 10);
        TASK_ID t1 = sample->addTask(1, 1, 2);
        TASK_ID t2 = sample->addTask(4, 3, 3);
        TASK_ID t3 = sample->addTask(3, 3, 10);

        DEPENDENCY_ID d0 = sample->addDependency(t0, t1);
        DEPENDENCY_ID d1 = sample->addDependency(t0, t2);
        DEPENDENCY_ID d2 = sample->addDependency(t1, t3);

        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << generateAlpha(*sample, d0, 6, 5) << std::endl;

        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << generateAlpha(*sample, d1, 6, 5) << std::endl;

        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << generateAlpha(*sample, d2, 6, 5) << std::endl;

        std::cout << std::endl;
        std::cout << std::endl;


}

BOOST_AUTO_TEST_SUITE_END()
