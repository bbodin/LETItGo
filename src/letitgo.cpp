/*
 * letitgo.cpp
 *
 *  Created on: Dec 29, 2020
 *      Author: toky
 */

#include <letitgo.h>
#include <model.h>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stack>

PartialConstraintGraph
generate_partial_constraint_graph(const LETModel &model,
                                  const PeriodicityVector &K) {

  PartialConstraintGraph graph;

  for (Dependency d : model.dependencies()) {

    VERBOSE_INFO(" " << d);

    TASK_ID ti_id = d.getFirst();
    TASK_ID tj_id = d.getSecond();

    Task ti = model.getTaskById(ti_id);
    Task tj = model.getTaskById(tj_id);

    auto Ti = ti.getT();
    auto Tj = tj.getT();

    auto Di = ti.getD();
    auto Dj = tj.getD();

    auto ri = ti.getr();
    auto rj = tj.getr();

    EXECUTION_COUNT Ki = K[ti_id];
    EXECUTION_COUNT Kj = K[tj_id];

    auto gcdeT = std::gcd(Ti, Tj);
    auto gcdeK = std::gcd(Ti * Ki, Tj * Kj);
    auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

    VERBOSE_INFO("  "
                 << "gcdeT= " << gcdeT);
    VERBOSE_INFO("  "
                 << "gcdeK= " << gcdeK);
    VERBOSE_INFO("  "
                 << "Me= " << Me);

    for (auto ai = 1; ai <= Ki; ai++) {
      for (auto aj = 1; aj <= Kj; aj++) {

        VERBOSE_INFO("  "
                     << "from " << ai << " to " << aj);

        INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdeT;
        INTEGER_TIME_UNIT pi_min =
            std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdeK);
        INTEGER_TIME_UNIT pi_max =
            std::floor((-Me + Ti - alphae_ai_aj * gcdeT) / gcdeK);

        VERBOSE_INFO("   "
                     << "alphae_ai_aj= "
                     << "(" << Ti << "*" << ai << "-" << Tj << "*" << aj
                     << ") / " << gcdeT << " = " << alphae_ai_aj);
        VERBOSE_INFO("   "
                     << "pi_min= " << pi_min);
        VERBOSE_INFO("   "
                     << "pi_max= " << pi_max);

        if (pi_min <= pi_max) {
          // From Theorem 6 (ECRTS2020)
          INTEGER_TIME_UNIT Lmax =
              rj - ri + Ti - Tj - (pi_min * gcdeK + alphae_ai_aj * gcdeT);
          VERBOSE_INFO("   "
                       << "Lmax= " << Lmax);

          Execution ei(ti_id, ai);
          Execution ej(tj_id, aj);
          Constraint cij(ei, ej, Lmax);
          graph.add(cij);
        }
      }
    }
  }

  Execution s(-1, 0);
  Execution f(-1, 1);

  for (Task task : model.tasks()) {
    TASK_ID tid = model.getTaskIdByTask(task);
    auto Di = model.getTaskById(tid).getD();

    for (auto a = 1; a <= K[tid]; a++) {

      Execution t(tid, a);

      // for any t without pred add s-> t with weight 0
      if (graph.getInputs(t).size() == 0) {
        Constraint c(s, t, 0);
        graph.add(c);
      }
      // for any t without succ add t -> f with weight Di (i the task)
      if (graph.getOutputs(t).size() == 0) {
        Constraint c(t, f, Di);
        graph.add(c);
      }
    }
  }

  return graph;
}

std::vector<Execution> topologicalOrder(PartialConstraintGraph PKG) {

  std::map<Execution, std::set<Constraint>> outbounds_copy = PKG.outbounds;
  std::map<Execution, std::set<Constraint>> inbounds_copy = PKG.inbounds;
  std::vector<Execution> L;
  std::vector<Execution> S = {Execution(-1, 0)};

  while (S.size()) {
    Execution n = S[S.size() - 1];
    S.pop_back();
    L.push_back(n);

    for (Constraint e : outbounds_copy[n]) {
      Execution m = e.getDestination();
      // remove edge
      inbounds_copy[m].erase(e);

      if (inbounds_copy[m].size() == 0) {
        S.push_back(m);
      }
    }
    outbounds_copy[n].clear();
  }

  // assert all edges gone
  return L;
}

std::pair<std::vector<Execution>, INTEGER_TIME_UNIT>
FindLongestPath(PartialConstraintGraph PKG) {

  std::map<Execution, WEIGHT> dist;
  std::map<Execution, Execution> prev;

  dist[Execution(-1, 0)] = 0;

  std::vector<Execution> ordered_execution = topologicalOrder(PKG);

  for (Execution src : ordered_execution) {
    if (dist.count(src)) {
      for (Constraint c : PKG.getOutputs(src)) {
        Execution dest = c.getDestination();
        if (dist.count(dest) == 0 || dist[dest] < dist[src] + c.getWeight()) {
          dist[dest] = dist[src] + c.getWeight();

          auto const result = prev.insert(std::make_pair(dest, src));
          if (not result.second) {
            result.first->second = src;
          }

          VERBOSE_INFO(" Update " << dest << " by " << src);
          VERBOSE_INFO(" prev =  " << prev);
        }
      }
    } else {
      VERBOSE_ERROR("Topological order failed");
    }
  }

  for (Execution e : ordered_execution) {
    VERBOSE_ASSERT(dist.count(e), "Could not find dist for execution" << e);
    VERBOSE_INFO(e << " distance is " << dist[e]);
  }

  VERBOSE_INFO(prev);

  std::vector<Execution> L;
  Execution e = Execution(-1, 1);
  while (prev.count(e) == 1) {
    VERBOSE_INFO("Longest to " << e << " comes from " << prev.at(e));
    L.push_back(e);
    e = prev.at(e);
  }
  L.push_back(e);

  std::reverse(L.begin(), L.end());

  return std::pair<std::vector<Execution>, INTEGER_TIME_UNIT>(
      L, dist[Execution(-1, 1)]);
}

INTEGER_TIME_UNIT ComputeAgeLatency(const LETModel &model) {
  INTEGER_TIME_UNIT res = 0;
  bool NeedsToContinue = true;
  PeriodicityVector K = generate_unitary_periodicity_vector(model);

  while (NeedsToContinue) {

    PartialConstraintGraph PKG = generate_partial_constraint_graph(model, K);
    auto FLP = FindLongestPath(PKG);
    auto P = FLP.first;
    res = FLP.second;
    VERBOSE_INFO("FindLongestPath(PKG) = " << FLP);
    INTEGER_TIME_UNIT T_P = 1;
    std::map<TASK_ID, INTEGER_TIME_UNIT> N;
    for (Execution e : P) {
      if (e.first == -1)
        continue;
      auto tid = e.getTaskId();
      auto task = model.getTaskById(tid);
      auto taskT = task.getT();
      T_P = std::lcm(T_P, taskT);
    }
    for (Execution e : P) {
      if (e.first == -1)
        continue;
      auto tid = e.getTaskId();
      auto task = model.getTaskById(tid);
      auto taskT = task.getT();
      N[tid] = (T_P / taskT);
    }

    NeedsToContinue = false;
    for (Execution e : P) {
      if (e.first == -1)
        continue;
      auto tid = e.getTaskId();
      if ((K[tid] % N[tid]) != 0) {
        NeedsToContinue = true;
        VERBOSE_INFO("NiP|Ki = " << N[tid] << "|" << K[tid]
                                 << " needs to continue ..");
      } else {
        VERBOSE_INFO("NiP|Ki = " << N[tid] << "|" << K[tid]);
      }
    }

    if (!NeedsToContinue)
      continue;

    for (Execution e : P) {
      if (e.first == -1)
        continue;
      auto tid = e.getTaskId();
      K[tid] = std::lcm(K[tid], N[tid]);
      VERBOSE_INFO("NiP|Ki = " << N[tid] << "|" << K[tid] << " will be "
                               << std::lcm(K[tid], N[tid]));
    }
  }

  VERBOSE_INFO("Final K = " << K);
  VERBOSE_INFO("Final Delay = " << res);

  return res;
}
