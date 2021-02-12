
INCLUDE_DIRECTORIES(${LETITGO_INCLUDE_DIR})

ADD_LIBRARY			   (letitgo SHARED   letitgo.cpp  generator.cpp verbose.cpp)
