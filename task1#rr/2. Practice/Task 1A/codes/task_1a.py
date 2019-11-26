
'''
*****************************************************************************************
*
*        		===============================================
*           		Rapid Rescuer (RR) Theme (eYRC 2019-20)
*        		===============================================
*
*  This script is to implement Task 1A of Rapid Rescuer (RR) Theme (eYRC 2019-20).
*
*  This software is made available on an "AS IS WHERE IS BASIS".
*  Licensee/end user indemnifies and will keep e-Yantra indemnified from
*  any and all claim(s) that emanate from the use of the Software or
*  breach of the terms of this agreement.
*
*  e-Yantra - An MHRD project under National Mission on Education using ICT (NMEICT)
*
*****************************************************************************************
'''


# Team ID:			[ Team-ID ]
# Author List:		[ Names of team members worked on this file separated by Comma: Name1, Name2, ... ]
# Filename:			task_1a.py
# Functions:		readImage, solveMaze
# 					[ Comma separated list of functions in this file ]
# Global variables:	CELL_SIZE
# 					[ List of global variables defined in this file ]


# Import necessary modules
# Do not import any other modules
import cv2
import numpy as np
import os


# To enhance the maze image
import image_enhancer


# Maze images in task_1a_images folder have cell size of 20 pixels
CELL_SIZE = 20


def readImage(img_file_path):

	"""
	Purpose:
	---
	the function takes file path of original image as argument and returns it's binary form

	Input Arguments:
	---
	`img_file_path` :		[ str ]
		file path of image

	Returns:
	---
	`original_binary_img` :	[ numpy array ]
		binary form of the original image at img_file_path

	Example call:
	---
	original_binary_img = readImage(img_file_path)

	"""

	binary_img = None

	#############	Add your Code here	###############

	#load the image in greyscale
	img = cv2.imread(img_file_path,0)
	#converting to binary image
	ret,binary_img = cv2.threshold(img,127,255,cv2.THRESH_BINARY)
	"""
	cv2.imshow("binary",binary_img)
	cv2.waitKey(0)
	cv2.destroyAllWindows()
	"""
	###################################################

	return binary_img


def solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width):

	"""
	Purpose:
	---
	the function takes binary form of original image, start and end point coordinates and solves the maze
	to return the list of coordinates of shortest path from initial_point to final_point

	Input Arguments:
	---
	`original_binary_img` :	[ numpy array ]
		binary form of the original image at img_file_path
	`initial_point` :		[ tuple ]
		start point coordinates
	`final_point` :			[ tuple ]
		end point coordinates
	`no_cells_height` :		[ int ]
		number of cells in height of maze image
	`no_cells_width` :		[ int ]
		number of cells in width of maze image

	Returns:
	---
	`shortestPath` :		[ list ]
		list of coordinates of shortest path from initial_point to final_point

	Example call:
	---
	shortestPath = solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width)

	"""


	shortestPath = []

	#############	Add your Code here	###############

	class Link():
		value = 0
		parent = 0
		def __init__(self,a,b):
			self.value = a
			self.parent = b


	def next_num():
		del x[0]
		return x[0]


	def number_maze(graph , initial_point , new , l ):
		empty = 0
		lar = 0

		for k in graph[initial_point]:
			if new[tuple(k)] == -1:
				empty += 1
		if empty == 0:
			return
		elif empty == 1:
			for i in range(len(graph[initial_point])):
				if new[tuple(graph[initial_point][i])] == -1:
					new[tuple(graph[initial_point][i])] = new[initial_point]
					number_maze(graph, tuple(graph[initial_point][i]), new, l)
		else:
			for i in range(len(graph[initial_point])):
				if new[tuple(graph[initial_point][i])] == -1:
					n = next_num()
					new[tuple(graph[initial_point][i])] = n
					l.append(Link(n, new[initial_point]))
					number_maze(graph, tuple(graph[initial_point][i]), new, l)


	def shortest_numbered_path(l, initial_point , final_point , path):
		if final_point == initial_point:
			return
		else:
			for i in l:
				if i.value == final_point:
					parent = i.parent
					break
			path.append(parent)
			shortest_numbered_path(l , initial_point ,parent,path  )


	def find_path(graph, initial_point, final_point, img):
		shortest_path = [initial_point]
		visited = [initial_point]
		count = 1
		global path
		path = []
		global l
		l = []
		global new
		new = {}

		for k in list(graph.keys()):
			new[k] = -1
		new[initial_point] = 1
		global x
		x = list(range(1,1000))
		number_maze(graph, initial_point , new , l )
		path.append(new[final_point])
		shortest_numbered_path(l, new[initial_point], new[final_point], path)
		path.reverse()
		current_point = initial_point
		while(True):
			if current_point == final_point:
				return shortest_path
			for j in graph[current_point]:
				if not tuple(j) in visited:
					if new[tuple(j)] == new[current_point]:
						current_point = tuple(j)
						shortest_path.append(current_point)
						visited.append(current_point)
						break
					else:
						if new[tuple(j)] == path[count]:
							count += 1
							shortest_path.append(tuple(j))
							current_point = tuple(j)
							visited.append(current_point)
							break




	#algorithm for finding neighbours of the each cells
	def find_neighbours(img,row,column):
		neighbours = []
		new_row = (2*row+1)*int(CELL_SIZE/2)
		new_column = (2*column+1)*int(CELL_SIZE/2)

		top_of_cell = new_row - int(CELL_SIZE/2) +1
		bottom_of_cell = new_row + int(CELL_SIZE/2) -2
		left_of_cell = new_column - int(CELL_SIZE/2) +1
		right_of_cell = new_column + int(CELL_SIZE/2) -2

		if img[top_of_cell , new_column] != 0:
			neighbours.append([row-1,column])
		if img[bottom_of_cell, new_column] != 0:
			neighbours.append([row+1,column])
		if img[new_row,left_of_cell] != 0:
			neighbours.append([row, column-1])
		if img[new_row, right_of_cell] != 0:
			neighbours.append([row,column+1])


		#print("\nneighbours for cell ",row,",",column," are : ", neighbours)

		return neighbours




	#code for building graph
	#dictionary is used for the graph for the sake of  simplicity
	graph = {}
	for row in range(0,no_cells_height):
		for column in range(0,no_cells_width):
			graph[row,column] = find_neighbours(original_binary_img,row,column)



	shortestPath = find_path(graph,initial_point, final_point, original_binary_img)




	###################################################

	return shortestPath


#############	You can add other helper functions here		#############



#########################################################################


# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS FUNCTION
#
# Function Name:	main
# Inputs:			None
# Outputs: 			None
# Purpose: 			the function first takes 'maze00.jpg' as input and solves the maze by calling readImage
# 					and solveMaze functions, it then asks the user whether to repeat the same on all maze images
# 					present in 'task_1a_images' folder or not

if __name__ == '__main__':

	curr_dir_path = os.getcwd()
	img_dir_path = curr_dir_path + '/../task_1a_images/'				# path to directory of 'task_1a_images'

	file_num = 0
	img_file_path = img_dir_path + 'maze0' + str(file_num) + '.jpg'		# path to 'maze00.jpg' image file

	print('\n============================================')

	print('\nFor maze0' + str(file_num) + '.jpg')

	try:

		original_binary_img = readImage(img_file_path)
		height, width = original_binary_img.shape

	except AttributeError as attr_error:

		print('\n[ERROR] readImage function is not returning binary form of original image in expected format !\n')
		exit()

	no_cells_height = int(height/CELL_SIZE)							# number of cells in height of maze image
	no_cells_width = int(width/CELL_SIZE)							# number of cells in width of maze image
	initial_point = (0, 0)											# start point coordinates of maze
	final_point = ((no_cells_height-1),(no_cells_width-1))			# end point coordinates of maze

	try:

		shortestPath = solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width)

		if len(shortestPath) > 2:

			img = image_enhancer.highlightPath(original_binary_img, initial_point, final_point, shortestPath)

		else:

			print('\n[ERROR] shortestPath returned by solveMaze function is not complete !\n')
			exit()

	except TypeError as type_err:

		print('\n[ERROR] solveMaze function is not returning shortest path in maze image in expected format !\n')
		exit()

	print('\nShortest Path = %s \n\nLength of Path = %d' % (shortestPath, len(shortestPath)))

	print('\n============================================')

	cv2.imshow('canvas0' + str(file_num), img)
	cv2.waitKey(0)
	cv2.destroyAllWindows()


	choice = input('\nWant to run your script on all maze images ? ==>> "y" or "n": ')

	if choice == 'y':

		file_count = len(os.listdir(img_dir_path))

		for file_num in range(file_count):

			img_file_path = img_dir_path + 'maze0' + str(file_num) + '.jpg'

			print('\n============================================')

			print('\nFor maze0' + str(file_num) + '.jpg')

			try:

				original_binary_img = readImage(img_file_path)
				height, width = original_binary_img.shape

			except AttributeError as attr_error:

				print('\n[ERROR] readImage function is not returning binary form of original image in expected format !\n')
				exit()

			no_cells_height = int(height/CELL_SIZE)							# number of cells in height of maze image
			no_cells_width = int(width/CELL_SIZE)							# number of cells in width of maze image
			initial_point = (0, 0)											# start point coordinates of maze
			final_point = ((no_cells_height-1),(no_cells_width-1))			# end point coordinates of maze

			try:

				shortestPath = solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width)

				if len(shortestPath) > 2:

					img = image_enhancer.highlightPath(original_binary_img, initial_point, final_point, shortestPath)

				else:

					print('\n[ERROR] shortestPath returned by solveMaze function is not complete !\n')
					exit()

			except TypeError as type_err:

				print('\n[ERROR] solveMaze function is not returning shortest path in maze image in expected format !\n')
				exit()

			print('\nShortest Path = %s \n\nLength of Path = %d' % (shortestPath, len(shortestPath)))

			print('\n============================================')
			
			cv2.imshow('canvas0' + str(file_num), img)
			cv2.waitKey(0)
			cv2.destroyAllWindows()

	else:

		print('')
