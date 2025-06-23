#Gate 2D Histogram File and Build Truth Table File
#Program by: Cade Saugier, INPP EAL Ohio University

import sys
import time
from matplotlib import pyplot
from matplotlib import colors
from matplotlib import path
from matplotlib.widgets import LassoSelector

#Build Main
def main():
	###Open Data File and Pull
	dataFile=open('./'+sys.argv[1],'r')
	global data
	data=pull3(dataFile)
	dataFile.close()
	
	###Get File Name
	k=0
	for i in range(len(sys.argv[1])):
		if sys.argv[1][i]=='/':
			k=i
	k+=1
	dataFileName=sys.argv[1][k:]
	
	###Build 2D Hist. Plot
	fig=pyplot.figure(1)
	plotAx=fig.add_subplot(1,1,1)
	draw=plotAx.imshow(data,cmap='viridis',norm=colors.LogNorm(),origin='lower')
	fig.colorbar(draw,ax=plotAx)
	pyplot.xlabel(sys.argv[3])
	pyplot.ylabel(sys.argv[2])
	pyplot.title(dataFileName)
	
	###Build Pixel Array
	global pix
	pix=[]
	i=0
	while i<len(data):
		for j in range(len(data)):
			pix+=[[j,i]]
		i+=1
	
	###Build Truth Lists
	xTruth=[]
	yTruth=[]
	for i in range(65536):
		xTruth+=[False]
		yTruth+=[False]
	
	###Run Gate Ask
	lass=LassoSelector(plotAx,take)
	
	###Save Figure
	pyplot.savefig('./output_'+sys.argv[4]+'/gate_'+sys.argv[2]+'-'+sys.argv[3]+'.pdf',format='pdf')
	
	###Plot Data
	pyplot.show()
	
	print('\n>>> Processing <<<')
	
	###Set Truth Lists
	size=len(data)
	chunk=int(65536/size)
	for k in range(len(truth)):
		if truth[k]:
			for w in range(chunk):
				yTruth[pix[k][1]*chunk+w]=True
				xTruth[pix[k][0]*chunk+w]=True
	
	###Write Truth Files
	xOut=open('./output_'+sys.argv[4]+'/x.truth','w')
	yOut=open('./output_'+sys.argv[4]+'/y.truth','w')
	for i in range(65536):
		if yTruth[i]:
			yOut.write('1\n')
		else:
			yOut.write('0\n')
		if xTruth[i]:
			xOut.write('1\n')
		else:
			xOut.write('0\n')
	xOut.close()
	yOut.close()
	
	print('>>> Complete! <<<')

############# FUNCTION LIST #############
def pull3(data):
	#Pull X, Y, and Z Data
	read=data.readlines()
	x=[]
	y=[]
	z=[]
	for i in range(len(read)):
		space1=0
		space2=0
		for j in range(len(read[i])):
			if read[i][j]==' ' and space1==0:
				space1=j
			elif read[i][j]==' ' and space1!=0:
				space2=j
		y+=[int(read[i][:space1])]
		x+=[int(read[i][space1+1:space2])]
		z+=[int(read[i][space2+1:])]
	xSize=max(x)
	ySize=max(y)
	if 0 in x:
		xSize+=1
	if 0 in y:
		ySize+=1
	mat=[]
	for i in range(xSize):
		mat+=[[]]
		for j in range(ySize):
			mat[i]+=[0]
	k=0
	for i in range(xSize):
		for j in range(ySize):
			mat[i][j]+=z[k]
			k+=1
	return mat

def take(verts):
	print('--->Loading Path Selection...')
	loop=path.Path(verts)
	global truth
	truth=loop.contains_points(pix,radius=0.0)
	print('--->Done!')
	time.sleep(2)
	pyplot.close()



#Run Main()
main()
