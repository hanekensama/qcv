#include <iostream>
#include "qcv.hpp"

using namespace std;

int main() {
  Circuit c;
  c.add_gate("CNOT", 0, 1);

  cout << "IX: " << c.simulate(Stabilizer("IX")) << endl;
  cout << "XI: " << c.simulate(Stabilizer("XI")) << endl;
  cout << "IZ: " << c.simulate(Stabilizer("IZ")) << endl;
  cout << "ZI: " << c.simulate(Stabilizer("ZI")) << endl;

  return 0;
}
