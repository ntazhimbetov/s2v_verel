#ifndef VEREL_ENV_H
#define VEREL_ENV_H

#include "i_env.h"

extern int sign;

class VerelEnv : public IEnv
{
 public:

  VerelEnv(double _norm);

  virtual void s0(std::shared_ptr<Graph>  _g) override;

  virtual double step(int a) override;

  virtual int randomAction() override;

  virtual bool isTerminal() override;

  double eliminate_node(int new_node);

  int numEliminatedNodes;
  std::set<int> eliminated_set;
  std::vector<int> avail_list;
};

#endif
