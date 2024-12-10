/*
 * GeneratorTest.cpp
 *
 */

#define BOOST_TEST_MODULE GenerateSampleTest
#include <boost/test/unit_test.hpp>
#include <letitgo.h>

#include <benchmark.h>

#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

#include <sstream>
#include <string>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

std::string generateLatexSchedule(const LETModel &model, TIME_UNIT hyperPeriod) {
    std::ostringstream latex;

    // Start the figure
    latex << "\\begin{figure}\n\n"
          << "  \\begin{LET}{" << model.getTaskCount() << "}{" << hyperPeriod << "}\n";

    // Add tasks
    for (const auto &task : model.tasks()) {
        TASK_ID id = task.getId();
        TIME_UNIT r = task.getr();
        TIME_UNIT C = task.getC() * 0.8;
        TIME_UNIT D = task.getD();
        INTEGER_TIME_UNIT T = task.getT();
        latex << "    \\LETTask{" << id << "}{" << r << "}{" << C << "}{" << D << "}{" << T << "}\n";
    }

    // Perform symbolic execution for dependencies
    for (const auto &dependency : model.dependencies()) {
        TASK_ID sourceId = dependency.getFirst();
        TASK_ID destId = dependency.getSecond();

        const Task &sourceTask = model.getTaskById(sourceId);
        const Task &destTask = model.getTaskById(destId);

        INTEGER_TIME_UNIT sourcePeriod = sourceTask.getT();
        INTEGER_TIME_UNIT destPeriod = destTask.getT();
        TIME_UNIT sourceRelease = sourceTask.getr();
        TIME_UNIT destRelease = destTask.getr();

        // Simulate execution from 0 to hyperperiod for the destination task
        for (INTEGER_TIME_UNIT destExecutionIndex=1, t = destRelease; t <= hyperPeriod; t=t+destPeriod, destExecutionIndex++) {
                // Destination task is executed
                // Find the latest execution of the source task that could affect this execution

                INTEGER_TIME_UNIT mostRecentIndex = 0;
                INTEGER_TIME_UNIT mostRecentStart = 0;
                for (INTEGER_TIME_UNIT sourceExecutionIndex=1, st = sourceRelease; st + sourceTask.getD() <= t; st=st+sourcePeriod, sourceExecutionIndex++) {
                    mostRecentIndex = sourceExecutionIndex;
                    mostRecentStart = st;
                }

                if ((mostRecentIndex > 0) && (t  < hyperPeriod)) {
                    // Add the dependency
                    latex << "    \\LETExecutionDependency{"
                    << sourceId << "}{" << mostRecentIndex << "}{"
                    << destId   << "}{" << destExecutionIndex << "}";

                    latex << " %  Destination starts at " << t << "\n";

                }
        }
    }

    // End the tikzpicture
    latex << "  \\end{LET}\n\n"
          << "  \\caption{Example of a LET application with " << model.getTaskCount()
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
          << "\\centering\n"
          << "\\begin{tikzpicture}[scale=0.90]\n";

    // Add nodes
    for (const auto &task : model.tasks()) {
        latex << "\\node[circle,draw] (" << task.getId()  << ") at ("
              << task.getId() * 2 << ","
              << ((task.getId() % 2 == 0) ? 1 : -1) << ") {$t_" << task.getId()  << "$};\n";
    }

    // Add edges
    for (const auto &dependency : model.dependencies()) {
        latex << "\\draw[->]  (" << dependency.getFirst()  << ") -- (" << dependency.getSecond()  << ");\n";
    }

    latex << "\\end{tikzpicture}\n"
          << "\\columnbreak\n\n"
          << "\\begin{center}\n"
          << "\\begin{tabular}{|c|c|c|c|c|}\n"
          << "\\hline\n"
          << "$t_i$ &";

    // Task IDs
    for (const auto &task : model.tasks()) {
        latex << " $t_" << task.getId()  << "$ &";
    }
    latex.seekp(-1, std::ios_base::end);
    latex << "\\\\\\hline\n$r_i$ &";

    // Release times
    for (const auto &task : model.tasks()) {
        latex << " $" << task.getr() << "$ &";
    }
    latex.seekp(-1, std::ios_base::end);
    latex << "\\\\\\hline\n$D_i$ &";

    // Deadlines
    for (const auto &task : model.tasks()) {
        latex << " $" << task.getD() << "$ &";
    }
    latex.seekp(-1, std::ios_base::end);
    latex << "\\\\\\hline\n$T_i$ &";

    // Periods
    for (const auto &task : model.tasks()) {
        latex << " $" << task.getT() << "$ &";
    }
    latex.seekp(-1, std::ios_base::end);
    latex << "\\\\\\hline\n"
          << "\\end{tabular}\n"
          << "\\end{center}\n"
          << "\\end{multicols}\n"
          << "\\caption{An instance of $" << model.getTaskCount() << "$ periodic tasks and the associated DAG ${\\cal G}$.}\n"
          << "\\label{fig:example_dag}\n"
          << "\\end{figure}\n";

    return latex.str();
}
std::string generateFirstTaskLatexDescription(const LETModel &model) {
    std::ostringstream latex;

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

    return latex.str();
}

#include <sstream>
#include <cmath>
#include <numeric> // for std::gcd

std::string generateDependencyInfoLatex(const LETModel &model, DEPENDENCY_ID d) {
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

    // Compute specific examples for ν_1 = 1 and ν_1 = 2
    for (int nu1 = 1; nu1 <= 2; ++nu1) {
        std::vector<int> nu2Values;
        for (int nu2 = 1; nu2 <= 50; ++nu2) { // Arbitrary limit for nu2 search
            TIME_UNIT condition = Me + srcT * nu1 - destT * nu2;
            if (condition > 0 && condition <= srcT) {
                nu2Values.push_back(nu2);
            }
        }

        if (nu1 == 1) {
            latex << "For example, setting $\\nu_{" << dep.getFirst() << "}=" << nu1 << "$, we obtain ";
        } else {
            latex << "Similarly, for $\\nu_{" << dep.getFirst() << "}=" << nu1 << "$, we get ";
        }

        latex << "$\\nu_{" << dep.getSecond() << "}\\in\\{";
        for (size_t i = 0; i < nu2Values.size(); ++i) {
            if (i > 0) latex << ",";
            latex << nu2Values[i];
        }
        latex << "\\}$";

        if (nu1 == 1) {
        latex << " associated to the dependencies $\\Exec{t_{" << dep.getFirst() << "}}{" << nu1
              << "}\\rightarrow \\Exec{t_{" << dep.getSecond() << "}}{\\nu_{" << dep.getSecond()
              << "}}$.";
        } else {
            latex << ".";
        }
        latex << std::endl;
    }

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

    std::cout << generateDependencyInfoLatex(*sample, d1)  << std::endl;

    std::cout << generateLatexFigure(*sample)  << std::endl;

    std::cout <<    generateFirstTaskLatexDescription(*sample)<< std::endl;

    std::cout <<    generateLatexSchedule(*sample, 29) << std::endl;
}

    BOOST_AUTO_TEST_CASE(test_generate_old) {

        auto sample = new LETModel();

        TASK_ID t2 = sample->addTask(4, 3, 3);
        TASK_ID t0 = sample->addTask(1, 4, 10);
        TASK_ID t1 = sample->addTask(1, 1, 2);
        TASK_ID t3 = sample->addTask(3, 3, 10);

        DEPENDENCY_ID d0 = sample->addDependency(t0, t1);
        DEPENDENCY_ID d1 = sample->addDependency(t0, t2);
        DEPENDENCY_ID d2 = sample->addDependency(t1, t3);

        std::cout <<    generateFirstTaskLatexDescription(*sample)<< std::endl;

        std::cout << generateLatexFigure(*sample)  << std::endl;

        std::cout << generateDependencyInfoLatex(*sample, d0)  << std::endl;




        std::cout <<    generateLatexSchedule(*sample, 29) << std::endl;


        std::cout << sample.
    }

BOOST_AUTO_TEST_SUITE_END()
