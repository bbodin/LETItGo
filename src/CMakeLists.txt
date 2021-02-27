
INCLUDE_DIRECTORIES(${LETITGO_INCLUDE_DIR})

ADD_LIBRARY			   (letitgo SHARED   age_latency.cpp  generator.cpp verbose.cpp partial_constraint_graph.cpp new_partial_graph_generation.cpp)
