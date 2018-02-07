#pragma once
#include <iostream>
#include <vector>
#include <string>

class Pauli {
private:
  char val;

public:
  Pauli(char val = 'I') {
    if (val == 'X' || val == 'Y' || val == 'Z' || val == 'I')
      this->val = val;
    else {
      std::cerr << "[Warn] unknown Pauli operator " << val << std::endl;
      this->val = 'I';
    }
  }

  Pauli(const Pauli&) = default;
  Pauli(Pauli&&) = default;
  Pauli& operator=(Pauli&&) = default;

  Pauli& operator*=(const Pauli& other) {
    if (this->val == 'I')
      this->val = other.val;
    else if (other.val != 'I') {
      if (this->val == other.val)
        this->val = 'I'; 
      switch(this->val) {
      case 'X':
        this->val = (other.val == 'Y') ? 'Z' : 'Y';
        break;
      case 'Y':
        this->val = (other.val == 'Z') ? 'X' : 'Z';
        break;
      case 'Z':
        this->val = (other.val == 'X') ? 'Y' : 'X';
      }
    }

    return *this;
  }

  bool operator==(const Pauli& other) {
    return this->val == other.val;
  }
  bool operator==(char c) {
    return this->toChar() == c;
  }

  char toChar() const {
    return this->val;
  }
};

Pauli operator*(const Pauli& lhs, const Pauli& rhs) {
  auto product = Pauli(lhs);
  product *= rhs;
  return product;
}

std::ostream& operator<<(std::ostream& os, const Pauli& rhs) {
  os << rhs.toChar();
  return os;
}

class Stabilizer {
private:
  std::vector<Pauli> paulis;
  
public:
  Stabilizer(const std::string& str) {
    int size = str.size();
    this->paulis = std::vector<Pauli>(size);
    for (int i = 0; i < size; ++i) {
      this->paulis[i] = Pauli(str[i]);
    }
  }

  Stabilizer(const Stabilizer&)  = default;
  Stabilizer(Stabilizer&&) = default;
  Stabilizer& operator=(Stabilizer&&) = default;

  size_t size() const {
    return this->paulis.size();
  }

  Pauli& operator[](unsigned index) {
    return (this->paulis[index]);
  }

  Stabilizer& operator*=(const Stabilizer& other) {
    int i = 0;
    while (i < this->size() && i < other.size()) {
      this->paulis[i] *= other.paulis[i];
      i++;
    }
    return *this;
  }

  bool operator==(const Stabilizer& other) {
    return (toString() == other.toString());
  }

  std::string toString() const {
    std::string str;

    for (auto p : this->paulis)
      str += p.toChar();

    return str;
  }

};

Stabilizer operator*(const Stabilizer& lhs, const Stabilizer& rhs) {
  auto product = Stabilizer(lhs);
  product *= rhs;
  return product;
}

std::ostream& operator<<(std::ostream& os, const Stabilizer& obj) {
  os << obj.toString();
  return os;
}

class Gate {
public:
  virtual Stabilizer apply(Stabilizer S) = 0;
  virtual std::string toString() const = 0;
};

class CNOT : public Gate {
private:
  int c_bit, t_bit;

public:
  CNOT(int c_bit, int t_bit) : c_bit(c_bit), t_bit(t_bit) {
  }

  Stabilizer apply(Stabilizer S) {
    if (S[c_bit] == 'X' || S[c_bit] == 'Y') 
      S[t_bit] *= Pauli('X');

    if (S[t_bit] == 'Z' || S[t_bit] == 'Y') 
      S[c_bit] *= Pauli('Z');

    return S;
  }

  std::string toString() const {
    std::string str = "CNOT(";
    str += std::to_string(c_bit);
    str += ", ";
    str += std::to_string(t_bit);
    str += ")";
    return str;
  }

  int control() const {
    return c_bit;
  }
  int target() const {
    return t_bit;
  }
};

class CZ : public Gate {
private:
  int c_bit, t_bit;

public:
  CZ(int c_bit, int t_bit) : c_bit(c_bit), t_bit(t_bit) {}

  Stabilizer apply(Stabilizer S) {
    if (S[c_bit] == 'Z' || S[c_bit] == 'Y')
      S[t_bit] *= Pauli('Z');
    if (S[t_bit] == 'X' || S[t_bit] == 'Y')
      S[c_bit] *= Pauli('X');

    return S;
  }

  std::string toString() const {
    std::string str = "CZ(";
    str += std::to_string(c_bit);
    str += ", ";
    str += std::to_string(t_bit);
    str += ")";
    return str;
  }
};

class H : public Gate {
private:
  int t_bit;
  
public:
  H(int t_bit) : t_bit(t_bit){}

  Stabilizer apply(Stabilizer S) {
    if (S[t_bit] == 'X')
      S[t_bit] = Pauli('Z');
    else if (S[t_bit] == 'Z')
      S[t_bit] = Pauli('X');

    return S;
  }

  std::string toString() const {
    std::string str = "H(";
    str += std::to_string(t_bit);
    str += ")";
    return str;
  }
};
  
class S : public Gate {
private:
  int t_bit;

public:
  S(int t_bit) : t_bit(t_bit){}

  Stabilizer apply(Stabilizer S) {
    if (S[t_bit] == 'Z')
      S[t_bit] = Pauli('Y');
    else if(S[t_bit] == 'Y')
      S[t_bit] = Pauli('Z');

    return S;
  }

  std::string toString() const {
    std::string str = "S(";
    str += std::to_string(t_bit);
    str += ")";
    return str;
  }
};

class XZ_Collection : public Gate {
private:
  int c_bit, t_bit;

public:
  XZ_Collection(int c_bit, int t_bit) : c_bit(c_bit), t_bit(t_bit) {}

  Stabilizer apply(Stabilizer S) {
    if (S[c_bit] == 'Z' || S[c_bit] == 'Y')
      S[t_bit] *= Pauli('Z');
    S[c_bit] = Pauli('I');
  }

  std::string toString() const {
    std::string str = "XZ(";
    str += std::to_string(c_bit);
    str += ", ";
    str += std::to_string(t_bit);
    str += ")";
    return str;
  }
};

class ZX_Collection : public Gate {
private:
  int c_bit, t_bit;

public:
  ZX_Collection(int c_bit, int t_bit) : c_bit(c_bit), t_bit(t_bit) {}

  Stabilizer apply(Stabilizer S) {
    if (S[c_bit] == 'X' || S[c_bit] == 'Y')
      S[t_bit] *= Pauli('X');
    S[c_bit] = Pauli('I');
  }

  std::string toString() const {
    std::string str = "ZX(";
    str += std::to_string(c_bit);
    str += ", ";
    str += std::to_string(t_bit);
    str += ")";
    return str;
  }
};

class Circuit {
private:
  std::vector<Gate*> gates;
  int max_bit;

public:
  Circuit() : max_bit(0) {};
  
  Circuit& add_gate(std::string type, unsigned t_bit, unsigned c_bit = 0) {
    max_bit = std::max(max_bit, std::max(t_bit, c_bit));
    
    if (type == "CNOT") gates.push_back(new CNOT(c_bit, t_bit));
    else if (type == "CZ") gates.push_back(new CZ(c_bit, t_bit));
    else if (type == "XZ") gates.push_back(new XZ_Collection(c_bit, t_bit));
    else if (type == "ZX") gates.push_back(new ZX_Collection(c_bit, t_bit));
    else if (type == "H") gates.push_back(new H(t_bit));
    else if (type == "S") gates.push_back(new S(t_bit));
    else {
      std::cerr << "[Warn] Unknown gate type " << type << std::endl;
    }

    return *this;
  }

  Circuit& expandCNOT() {
    std::vector<int> map(numBits() + 2 * numGates(), -1); // ビットの写像を入れる．

    for (auto& g : gates) {
      if (decltype(g) == decltype(CNOT())) {
        int c_in = g.control();
        while (map[c_in] != -1) c_in = map[c_in];
        int t_in = g.target();
        while (map[t_in] != -1) t_in = map[t_in];

        int c_out = ++max_bit;
        int ancilla = ++max_bit;

        CNOT(ancilla, c_in);
        CNOT(ancilla, t_in);
        CNOT(ancilla, c_out);
        
        
      }
      
    }
  }

  unsigned numBits() const {
    return max_bit + 1;
  }

  unsigned numGates() const {
    return gates.size();
  }

  Stabilizer simulate(Stabilizer S) {
    for (auto g : gates) {
      S = g->apply(S);
    }
    return S;
  }
  
};
