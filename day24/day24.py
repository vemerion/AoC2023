from sympy import solve, symbols
import sys

def equation(pos, vel, time, const_pos, const_vel):
  return "{pos} + {vel} * t{time} - {const_pos} - {const_vel} * t{time}".format(pos=pos, vel=vel, time=time, const_pos=const_pos, const_vel=const_vel)

equations = []
i = 1
for line in sys.stdin:
  tokens = list(filter(None, line.replace(',', '').replace('@', '').replace('\n', '').split(' ')))
  equations.append(equation('x', 'vx', i, int(tokens[0]), int(tokens[3])))
  equations.append(equation('y', 'vy', i, int(tokens[1]), int(tokens[4])))
  equations.append(equation('z', 'vz', i, int(tokens[2]), int(tokens[5])))
  i += 1
  if i == 4:
    break

x, y, z, vx, vy, vz, t1, t2, t3 = symbols('x y z vx vy vz t1 t2 t3')
result = solve(equations, [x, y, z, vx, vy, vz, t1, t2, t3], dict=True)
print("Part 2 result = {}".format(result[0][x] + result[0][y] + result[0][z]))
