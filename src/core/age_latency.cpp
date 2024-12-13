/*
 * age_latency.cpp
 *
 *  Created on: Dec 29, 2020
 *      Author: toky
 */

#include <model.h>
#include <partial_constraint_graph.h>
#include <repetition_vector.h>
#include <utils.h>
#include <age_latency.h>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stack>
#include <chrono>



AgeLatencyResult compute_age_latency(const LETModel &model) {
    PEGOnCreatedFun hook = [](const PartialConstraintGraph &){return true;};
    return compute_age_latency_with_hook (model, hook);
}

/**
 * TODO: Topological order could be done si easily, it is the same as the original graph.
 * @param model
 * @return
 */
AgeLatencyResult compute_age_latency_with_hook(const LETModel &model, PEGOnCreatedFun & hook) {

    // Compute Global Repetition Vector
    auto repetition_vector = compute_repetition_vector(model);

    auto start = std::chrono::high_resolution_clock::now();

    //letitgo::utils::set_verbose_custom_mode ("AGE_LATENCY", true);
    VERBOSE_INFO ("Run compute_age_latency");
    AgeLatencyResult res(model);






    bool NeedsToContinue = true;
    PeriodicityVector K = generate_periodicity_vector(model);


    while (NeedsToContinue) {

        auto count = res.expansion_vertex_count.size();

        VERBOSE_INFO ("Iteration " << count<< " K = " << K << " N = " << repetition_vector);
        VERBOSE_INFO ("Iteration " << count<< " Graph Generation");

        auto s1 = std::chrono::high_resolution_clock::now();
        // Construct the PartialConstraintGraph and
        PartialConstraintGraph PKG = PartialConstraintGraph(model, K);
        auto s2 = std::chrono::high_resolution_clock::now();
        VERBOSE_ASSERT(hook(PKG), "Hook should return True");
        VERBOSE_INFO ("Iteration " << count  << " Find Longest Path");

        // Find the longest path and update the res
        auto s3 = std::chrono::high_resolution_clock::now();
        auto upper_bound = FindLongestPath(PKG, upper_wt);
        auto s4 = std::chrono::high_resolution_clock::now();

        auto P = upper_bound.first;


        VERBOSE_INFO ("Iteration " << count  << " Lower bound Graph Generation");
        // Compute the lower bound to check it is lower than the uppoer bound.
        VERBOSE_INFO ("Iteration " << count << " Lower bound Find Longest Path");
        auto lower_bound = FindLongestPath(PKG, lower_wt);
        VERBOSE_AGE_LATENCY(" * FindLongestPath(UPPER) = " << upper_bound);
        VERBOSE_AGE_LATENCY(" * FindLongestPath(LOWER) = " << lower_bound);
        VERBOSE_AGE_LATENCY(" * Check " << lower_bound.second << " <= " << upper_bound.second);
        VERBOSE_ASSERT(lower_bound.second <= upper_bound.second, "The lower bound function does not work");
        res.lower_bounds.push_back(lower_bound.second);


        auto s5 = std::chrono::high_resolution_clock::now();

        res.upper_bounds.push_back(upper_bound.second);
        res.expansion_vertex_count.push_back(PKG.getExecutionsCount());
        res.expansion_edge_count.push_back(PKG.getConstraintsCount());
        res.age_latency = upper_bound.second;

        res.graph_computation_time .push_back((s2-s1).count() / 1000000.0);
        res.upper_computation_time.push_back((s4-s3).count() / 1000000.0);
        res.lower_computation_time .push_back((s5-s4).count() / 1000000.0);



        VERBOSE_INFO ("Iteration " << count  << " Conclude");

        INTEGER_TIME_UNIT T_P = 1;


        for (Execution e : P) {
            if (e.getTaskId() == -1)
                continue;
            auto tid = e.getTaskId();
            auto task = model.getTaskById(tid);
            auto taskT = task.getT();
            VERBOSE_ASSERT(taskT > 0, "task.T Cannot be null");
            T_P = std::lcm(T_P, taskT);
        }

        VERBOSE_INFO ("Compute T_P = lcm (T in P) =" << T_P);
        VERBOSE_ASSERT(T_P > 0, "T_P Cannot be null");

        // Compute N[T]
        std::map<TASK_ID, INTEGER_TIME_UNIT> N;

        for (Execution e : P) {
            if (e.getTaskId() == -1)
                continue;
            auto tid = e.getTaskId();
            auto task = model.getTaskById(tid);
            auto taskT = task.getT();
            N[tid] = (T_P / taskT);
        }

        VERBOSE_INFO ("Compute N[ti] = T_P / T =" << N);

        // we check the critical cycle is max
        NeedsToContinue = false;
        for (Execution e : P) {
            if (e.getTaskId() == -1)
                continue;
            auto tid = e.getTaskId();
            if ((K[tid] % N[tid]) != 0) {
                NeedsToContinue = true;
                VERBOSE_AGE_LATENCY("NiP|Ki = " << N[tid] << "|" << K[tid]
                                                << " needs to continue ..");
            } else {
                VERBOSE_AGE_LATENCY("NiP|Ki = " << N[tid] << "|" << K[tid]);
            }
        }



        if (!NeedsToContinue) {
            VERBOSE_INFO ("Iteration " << count  << " Is the last one.");
            continue;
        }

        VERBOSE_INFO ("Iteration " << count  << " Update K");

        for (Execution e : P) {
            if (e.getTaskId() == -1)
                continue;
            auto tid = e.getTaskId();
            K[tid] = std::lcm(K[tid], N[tid]);
            VERBOSE_AGE_LATENCY("NiP|Ki = " << N[tid] << "|" << K[tid] << " will be "
                                            << std::lcm(K[tid], N[tid]));
        }
    }

    VERBOSE_AGE_LATENCY("Final K = " << K);
    VERBOSE_DEBUG("Final Age latency = " << res.age_latency);

    res.total_time = (std::chrono::high_resolution_clock::now()-start).count() / 1000000;
    return res;
}