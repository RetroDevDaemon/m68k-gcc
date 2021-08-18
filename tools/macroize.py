import sys 

if(len(sys.argv) < 2):
	print("Usage: \n\
$ python3 macroize.py /file/to/macro > /output/file\n\
\n\
Syntax:\n\
 %MACRONAME prm 1000\n\
\n\
 Where MACRONAME is any name, and 'prm' and '1000' are arbitrary parameter values\n\
\n\
 .macro \\name1 \\name2\n\
   print(name1, name2)\n\
 .endmacro\n\
\n\
 Where the given parameters are replaced with the values in the macro call.\n")
	sys.exit()

"""
Class definitions
"""

class Macro:
	'''Macro container'''

	parameters = [] # string array 
	name = ''		# string
	listing = []	# string array 

	def __init__(self):
		self.parameters = [] 
		self.name = '' 
		self.listing = []
		return

	def __str__(self):
		a = ''
		a = a + "Name: " + self.name + '\n'
		a = a + "Parameters: " + str(len(self.parameters)) + '\n'
		i = 0
		while i < len(self.listing):
			a = a + self.listing[i]
			i += 1
		return a
	#def FixListing(self):
	#	i = 0
	#	while i < len(self.listing):
	#		j = 0
	#		while j < len(self.parameters):
				#if(self.listing[i].find(self.parameters[j]) != -1):
					#print('Parameter ' + str(j) + 'on list \#' + str(i))
	#			j += 1
	#		i += 1
		#
	def copy(self):
		target = Macro()
		for f in self.parameters:
			target.parameters.append(f)
		target.name = self.name + ''
		for l in self.listing:
			target.listing.append(l)
		return target 
###

"""
Pass one: 
 Find all .macros in the given file and store them in macro objects
"""

f = open(sys.argv[1], 'r')

listing = f.readlines()
f.close()

allmacros = []

i = 0
while i < len(listing):
	# look for .macro definition
	if(listing[i].find('.macro') != -1):
		curmacro = Macro()
		# save the macro name and parameters
		par = [x.strip() for x in listing[i].split(' ')]
		if((len(par) == 1) or (par[1] == '\n')): 
			print("Error: Macro needs a name!")
			break 
		if(len(par[1]) == 0):
			print("Error: Macro definition weird, check name and spaces")
			break 
		curmacro.name = par[1]
		pars = []
		if(len(par) > 2):
			k = 2
			illegal = False
			while k < len(par):
				if(par[k][0] != '\\'):
					print("Error: Macro definition illegal: use .macro MACRONAME \\arg1 .. \\argn")
					illegal = True
					break
				else:   # trim '\\' and '\n'
					s = par[k]
					if(len(s) < 3):
						illegal = True
						print("Error: Parameter name too short!")
						break
					pars.append(s[1:])
				k += 1 	
			if(illegal):
				print("Error on line " + str(i))
				break 
		curmacro.parameters = pars
		i += 1
		curmacro.listing = []
		while (listing[i].find('.endmacro') == -1):
			curmacro.listing.append(listing[i])
			i += 1
		if (curmacro.name != ''):
			#print(curmacro)
			allmacros.append(curmacro)
	i += 1

#print(len(allmacros))

"""
Pass two:
  Find all instances of allmacros[0..n], and replace in-line with the macro's 
  listing with the given variables replaced.
"""

newcalls=[]
i = 0
while i < len(listing): #search every line
	j = 0
	while j < len(allmacros): # for macro usage
		if (listing[i].find('%' + allmacros[j].name) != -1): # search for macro name
			thism = allmacros[j].copy()
			#print('Macro ' + str(j) + ' call found on line ' + str(i))
			# split macro call 
			mline = [ x.strip() for x in listing[i].split(' ')]
			# trim any empty entries
			calls = []
			c = 0
			while c < len(mline):  
				if mline[c] != '': 
					calls.append(mline[c]) 
				c += 1
			# make sure the macro call is valid
			if len(calls) != len(allmacros[j].parameters)+1:
				print('Error: Macro call ' + allmacros[j].name + ' failed. Check parameters?')
				j = 99
				i = 99999
			# valid call - replace params
			else: 
				tr = thism.listing
				g = 0
				# iterate through the copy (current call) and replace all params
				# in the listing with current copy parameter values 
				while g < len(thism.parameters):
					ln = 0
					while ln < len(tr):
						ts = tr[ln]
						if(ts.find(thism.parameters[g]) != -1):
							ts = ts.replace(thism.parameters[g], calls[g+1])
							thism.listing[ln] = ts
						ln += 1
					g += 1
				thism.index = i 
				newcalls.append(thism)
		j += 1
	i += 1

"""
 Print the fixed text file
"""

## Fix the listing
i = 0
while i < len(newcalls):
	listing[newcalls[i].index] = newcalls[i].listing
	i += 1
## Create an outstr
out = ''
i = 0
while i < len(listing):
	if(type(listing[i]) == str): # if its a normal string, print it
		if(listing[i].find(".macro") != -1): # is this a macro line?
			while(listing[i].find(".endmacro") == -1):
				i += 1 # skip til the end
		else:
			out += listing[i]
	if(type(listing[i]) == list): # otherwise, print each element
		j = 0
		while j < len(listing[i]):
			out += listing[i][j]
			j += 1
	i += 1
	
print(out) #stdout
