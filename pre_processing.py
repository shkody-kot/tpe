'''
Primary code by Avani Sathe
Code takes an image's thumbnail and the thumbnail of that image filtered
and alters the image such that a thumbnail of the image would be identical
to the filtered thumbnail by changing the minimal amount of pixels'''

from PIL import Image
import numpy as np

def blockwise_difference(img1, img2, block_y, block_x, block_size):
    cols, rows = img2.size
    #print(str(cols) + " pixels(width) by " + str(rows) + " pixels(height)")
    diff_matrix = []
    
    block_cols = cols // block_x
    block_rows = rows // block_y
    
    for row in range(block_rows):
        for col in range(block_cols):
            original_r = 0
            original_g = 0
            original_b = 0
            old_total = 0
           
            new_r = 0
            new_g = 0
            new_b = 0
            new_total = 0
            for pixel in range(block_size):
                row_pixel = pixel // block_x          #row of pixel (row_num is row of block)
                col_pixel = pixel % block_x           #col of pixel (col_num is col of block)
                position = row * width * block_y + row_pixel * width + col * block_x + col_pixel
                oldr, oldg, oldb = img1[position]
                newr, newg, newb = img2[position]
                
                original_r += oldr
                original_g += oldg
                original_b += oldb
                
                new_r += newr
                new_g += newg
                new_b += newb
            
            #get mean value
            old_total = (original_r // block_size, original_g // block_size, original_b // block_size)
            new_total = (new_r // block_size, new_g // block_size, new_b // block_size)
            difference = tuple(map(lambda i, j: i - j, new_total, old_total))
            diff_matrix.append(difference)
            
    return diff_matrix
            


def blockwiseDifference(img1, img2, block_y, block_x, block_size):
    cols, rows = img1.size
    print(str(cols) + " pixels(width) by " + str(rows) + " pixels(height)")
    diff_matrix = []
    
    block_cols = cols // block_x
    block_rows = rows // block_y

    for row in range(0, rows, block_y):
        for col in range(0, cols, block_x):
            block1 = np.array(img1.crop((col, row, col + block_cols, row + block_rows)))
            block_sum_img1 = [(np.sum(block1[:, :, channel]) / block_size) for channel in range(block1.shape[2])]
            print(block_sum_img1)
            block2 = np.array(img2.crop((col, row, col + block_cols, row + block_rows)))
            block_sum_img2 = [(np.sum(block2[:, :, channel]) / block_size) for channel in range(block2.shape[2])]
            print(block_sum_img2)
            sum_diff = [
                np.subtract(int(block_sum_img1[i]), int(block_sum_img2[i]))
                for i in range(len(block_sum_img1))
            ]
            diff_matrix.append(sum_diff)
    return diff_matrix


#distribute the difference of thumbnail blocks among the pixels of that block
def distribute_pixels(image, pixels_to_distribute):
    channels = image.split()
    
    #for each channel (r, g, b)
    for channel_num, channel in enumerate(channels):
        pixels = np.array(channel.getdata())
        num_to_distribute = pixels_to_distribute[channel_num]
        total_pixels = len(pixels)
        average_to_add = int(num_to_distribute / total_pixels)
        overflow_pixels = num_to_distribute % total_pixels
        manipulations = []
        for i, pixel in enumerate(pixels):
            new_pixel = pixel + average_to_add
            if i < overflow_pixels:
                new_pixel += 1
            if new_pixel > 255:
                manipulations.append((i, pixel, 255))
                pixels[i] = 255
            if new_pixel < 0:
                manipulations.append((i, pixel, 0))
                pixels[i] = 0
            else:
                manipulations.append((i, pixel, new_pixel))
                pixels[i] = new_pixel
        #channels[channel_num] = np.array(pixels).reshape(channel.size)
        channels[channel_num].putdata(pixels)
    return Image.merge(image.mode, channels), manipulations

#rewritten
def generate_o_dash(img, img2, width, height, block_x, block_y, diff_matrix):
    num_pixels = width * height
    pixels = [0] * num_pixels
    print("O_DASH")
    block_rows = height // block_y
    block_cols = width // block_x
    
    block_size = block_x * block_y
    
    #Loop over every block
    for row_num in range(block_rows):
        for col_num in range(block_cols):
            #get number of block
            block_num = row_num * block_cols + col_num
            
            #values to add
            add_red, add_green, add_blue = diff_matrix[block_num]
            add_red = add_red * block_size
            add_green = add_green * block_size
            add_blue = add_blue * block_size
            
            #print("red: " + str(add_red) + " green: " + str(add_green) + " blue: " + str(add_blue))
            #for each pixel in the block
            for pixel in range(block_x * block_y):
                row = pixel // block_x          #row of pixel (row_num is row of block)
                col = pixel % block_x           #col of pixel (col_num is col of block)
                position = row_num * width * block_y + row * width + col_num * block_x + col
                red, green, blue = img[position]
                
                #print("positiom " + str(position) + " -- " + str(img[position]))
                #print("old values: " + str(red) + " -- " + str(green) + " -- " + str(blue))
                
                #add to red channel
                if red + add_red > 255:
                    #print("red overflow in pixel " + str(position))
                    add_red -= (255 - red)
                    red = 255
                elif red + add_red < 0:
                    #print("red underflow in pixel " + str(position))
                    #print("old red: " + str(red))
                    add_red += red
                    red = 0
                    #print("After change red: " + str(red))
                else:
                    red += add_red
                    add_red = 0
                
                #add to green channel
                if green + add_green > 255:
                    #print("green overflow in pixel " + str(position))
                    add_green -= (255 - green)
                    green = 255
                elif green + add_green < 0:
                    #print("green underflow in pixel " + str(position))
                    add_green += green
                    green = 0
                else:
                    green += add_green
                    add_green = 0
                    
                #add to blue channel
                if blue + add_blue > 255:
                    #print("blue overflow in pixel " + str(position))
                    add_blue -= (255 - blue)
                    blue = 255
                elif blue + add_blue < 0:
                    #print("blue underflow in pixel " + str(position))
                    add_blue += blue
                    blue = 0
                else:
                    blue += add_blue
                    add_blue = 0
                
                #return values to image
                rgb = (red, green, blue)
                pixels[position] = rgb
    #test
    test = blockwise_difference(pixels, img2, block_y, block_x, block_x * block_y)
    print(test)
    #create image in right order
    pixels = np.reshape(pixels, (height, width, 3))
    print(np.shape(pixels))
    img2 = Image.fromarray(pixels.astype(np.uint8))
    img2.save("modified_image(ira).jpg")          

'''def generate_o_dash(img1, block_cols, block_rows, block_x, block_y, diff_matrix):
    # Loop over each block
    for by in range(block_rows):
        for bx in range(block_cols):
        # Calculate the starting and ending x and y coordinates of the block
            start_x = bx * block_x
            start_y = by * block_y
            end_x = start_x + block_x
            end_y = start_y + block_y
            print("row " + str(by) + " col " + str(bx))

            # Crop the block from the original image
            block1 = img1.crop((start_x, start_y, end_x, end_y))
            for i in range(len(diff_matrix)):
                #print("block " + str(i) + " row " + str(by) + " col " + str(bx))
                # Loop over each pixel in the block
                for y in range(block_y):
                    for x in range(block_x):
                        block1_after_modification, maniputations = distribute_pixels(block1, diff_matrix[i])
                        #Paste the modified block back onto the original image
                        img1.paste(block1_after_modification, (start_x, start_y, end_x, end_y))
    #Save the modified image
    img1.save("modified_image.jpg")'''

def create_metadata():
    
    return

#MAIN OPERATION

# Open the image (potentially make it accept stream of data as opposed to image url)
img1 = Image.open("B99_thumbnail.jpg")
img2 = Image.open("B99_mosaic_med.jpg")

'''
img1 = original_image_data
img2 = filtered_image_data (can maybe be thumbnail)
'''

# Get the width and height of the image
width, height = img1.size
print(str(width) + " pixels(width) by " + str(height) + " pixels(height)")

# Define the size of the block
block_x = 10
block_y = 14
block_size = block_x * block_y

print("calculated num pixels: " + str(width * height))


diff_matrix = blockwise_difference(img1.getdata(), img2.getdata(), block_y, block_x, block_size)

print(diff_matrix)
#print(len(diff_matrix))

generate_o_dash(img1.getdata(), img2.getdata(), width, height - 1, block_x, block_y, diff_matrix)

