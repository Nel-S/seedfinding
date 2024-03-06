from z3 import *
import csv

#Create constants
WEIGHT = -0.9

# Create variables
a = Real('a')
b = Real('b')
c = Real('c')

LCANDIDATES = {
	"l1": [-a - b, -a - c, -b - c],
	"l2": [a - b - 1, a - c - 1, -b - c],
	"l3": [b - a - 1, -a - c, b - c - 1],
	"l4": [a + b - 2, a - c - 1, b - c - 1],
	"l5": [-a - b, c - a - 1, c - b - 1],
	"l6": [a - b - 1, a + c - 2, c - b - 1],
	"l7": [b - a - 1, c - a - 1, b + c - 2],
	"l8": [a + b - 2, a + c - 2, b + c - 2],
}

# Create symbolic expressions
# l1 = Or(-a - b, -a - c, -b - c)
# l2 = Or(a - b - 1, a - c - 1, -b - c)
# l3 = Or(b - a - 1, -a - c, b - c - 1)
# l4 = Or(a + b - 2, a - c - 1, b - c - 1)
# l5 = Or(-a - b, c - a - 1, c - b - 1)
# l6 = Or(a - b - 1, a + c - 2, c - b - 1)
# l7 = Or(b - a - 1, c - a - 1, b + c - 2)
# l8 = Or(a + b - 2, a + c - 2, b + c - 2)

# l1 = -a - b
# l2 = a - b - 1
# l3 = b - a - 1
# l4 = a + b - 2
# l5 = -a - b
# l6 = a - b - 1
# l7 = b - a - 1
# l8 = a + b - 2

l1 = 0
l2 = 0
l3 = 0
l4 = 0
l5 = 0
l6 = 0
l7 = 0
l8 = 0

# Create constraints
s = Solver()
s.add(0 <= a, a < 1)
s.add(0 <= b, b < 1)
s.add(0 <= c, c < 1)
for a1 in LCANDIDATES['l1']:
	l1 = a1
	for a2 in LCANDIDATES['l2']:
		l2 = a2
		for a3 in LCANDIDATES['l3']:
			l3 = a3
			for a4 in LCANDIDATES['l4']:
				l4 = a4
				for a5 in LCANDIDATES['l5']:
					l5 = a5
					for a6 in LCANDIDATES['l6']:
						l6 = a6
						for a7 in LCANDIDATES['l7']:
							l7 = a7
							for a8 in LCANDIDATES['l8']:
								l8 = a8
								s.push()
								s.add(l1 + (6*a**5 - 15*a**4 + 10*a**3)*(l2 - l1) + (6*b**5 - 15*b**4 + 10*b**3)*(l3 - l1 + (6*a**5 - 15*a**4 + 10*a**3)*(l4 - l3 - l2 + l1)) + (6*c**5 - 15*c**4 + 10*c**3)*(l5 - l1 + (6*a**5 - 15*a**4 + 10*a**3)*(l6 - l5 - l2 + l1) + (6*b**5 - 15*b**4 + 10*b**3)*(l7 - l5 - l3 + l1 + (6*a**5 - 15*a**4 + 10*a**3)*(l8 - l7 - l6 + l5 - l4 + l3 + l2 - l1))) <= WEIGHT)
								print(str(a1), str(a2), str(a3), str(a4), str(a5), str(a6), str(a7), str(a8))
								while s.check() == sat:
									m = s.model()
									print("(a, b, c) = ", m[a], m[b], m[c])
									# Create a new constraint to exclude the current solution
									s.add(Or(a != m[a], b != m[b], c != m[c]))
								print("All current solutions found; continuing...")
								s.pop()

# Open a file for writing the solutions
# with open('solutions.csv', mode='w') as file:
#	 writer = csv.writer(file)
#	 writer.writerow(["a", "b", "c"]) #write the header
#	 # Find all solutions
#	 while s.check() == sat:
#		 m = s.model()
#		 # write the solution
#		 writer.writerow([m[a], m[b], m[c]])
#		 # Create a new constraint to exclude the current solution
#		 s.add(Or(a != m[a], b != m[b], c != m[c]))

print("Done.")
