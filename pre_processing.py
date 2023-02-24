'''
Initial code by Avani Sathe
Altered by Ira Belyaeva
Code takes an image's thumbnail and the thumbnail of that image filtered
and alters the image such that a thumbnail of the image would be identical
to the filtered thumbnail by changing the minimal amount of pixels'''

from PIL import Image
import numpy as np

#get an array of differences between means of two images' thumbnails
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
            
            #for each pixel in the block
            for pixel in range(block_x * block_y):
                row = pixel // block_x          #row of pixel (row_num is row of block)
                col = pixel % block_x           #col of pixel (col_num is col of block)
                position = row_num * width * block_y + row * width + col_num * block_x + col
                red, green, blue = img[position]
                
                #add to red channel
                if red + add_red > 255:
                    add_red -= (255 - red)
                    red = 255
                elif red + add_red < 0:
                    add_red += red
                    red = 0
                else:
                    red += add_red
                    add_red = 0
                
                #add to green channel
                if green + add_green > 255:
                    add_green -= (255 - green)
                    green = 255
                elif green + add_green < 0:
                    add_green += green
                    green = 0
                else:
                    green += add_green
                    add_green = 0
                    
                #add to blue channel
                if blue + add_blue > 255:
                    add_blue -= (255 - blue)
                    blue = 255
                elif blue + add_blue < 0:
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

