'''
Initial code by Avani Sathe
Altered by Ira Belyaeva
Code takes an image's thumbnail and the thumbnail of that image filtered
and alters the image such that a thumbnail of the image would be identical
to the filtered thumbnail by changing the minimal amount of pixels'''

from PIL import Image
import numpy as np
import os

#get an array of differences between means of two images' thumbnails, when no thumbnail is provided
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


#change pixels in the original image using a greedy algorithm to change each pixel
#as much as possible, therefore reducing metadata to store
def generate_o_dash(img, img2, width, height, block_x, block_y, diff_matrix, img_name):
    num_pixels = width * height
    pixels = [0] * num_pixels
    #store info about pixels changed
    changes = []
    last = (0, 0, 0)
    
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
                old_r = red
                old_g = green
                old_b = blue
                
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
                
                #check if any pixels have been changed
                if old_r != red or old_g != green or old_b != blue:
                    #append to changes list
                    red_tuple = (position, old_r, red)
                    green_tuple = (position, old_g, green)
                    blue_tuple = (position, old_b, blue)
                    changes.append(red_tuple)
                    changes.append(green_tuple)
                    changes.append(blue_tuple)
                
                #return values to image
                rgb = (red, green, blue)
                pixels[position] = rgb
    #test if successful
    #test = blockwise_difference(pixels, img2, block_y, block_x, block_x * block_y)
    #print(test)
    
    #generate metadata
    #m_height, metadata = create_metadata(changes, width)
    
    #append metadata to image data
    #metadata = np.reshape(metadata, (m_height, width, 3))    
    pixels = np.reshape(pixels, (height, width, 3))
    #pixels = np.append(pixels, metadata, 0)
    
    #create image in right order
    #pixels = np.reshape(pixels, (height + m_height, width, 3))
    print(np.shape(pixels))
    
    img_new = Image.fromarray(pixels.astype(np.uint8))
    img_new.save(img_name + "(encrypted).png")    

#create metadata for changed pixels
def create_metadata(changes, width):
    print("METADATA")
    metadata = [0] * len(changes)
    
    #sort the change list so it's by pixel position not by block
    changes.sort(key = lambda a: a[0])
    
    previous_change = 0
    
    
    #change the position value to the distance from last changed pixel, in hopes of
    #remaining with values less than 255
    for item in range(0, len(changes), 3):
        position, old, new = changes[item]
        temp = position
        
        for pixel in range(3):
            index, old, new = changes[item + pixel]
            index -= previous_change
            
            new_tuple = (index, old, new)
            metadata[item + pixel] = new_tuple
        
        #print("previous change: " + str(previous_change) + " new value: " + str(temp))
        previous_change = temp
    
    #see what i've done
    #print(metadata)
    
    #pad the metadata so it's a multiple of the width
    height = len(metadata) // width
    remainder = len(metadata) % width
    print("rows: " + str(height) + " remainder: " + str(remainder))
    
    for pixel in range(width - remainder):
        metadata.append((0, 0, 0))
    
    height += 1    
    return height, metadata

#undo o_dash pixel changes to restore original image
def undo_o_dash(img, width, height):
    #extract the metadata from image
    print("UNDO O_DASH")
    img_width, img_height = img.size
    
    metadata = [0] * (img_height - height) * width
    original_image = [0] * width * height
    
    m_row = 0
    
    #separate the matadata from the image
    for row in range(height, img_height):
        #print(row)
        for col in range(width):
            index = row * width + col
            
            position, old, new = img[index]
            
            #not a true change; padding
            if position == 0 and old == 0 and new == 0:
                continue
            
            new_tuple = (position, old, new)
            metadata[m_row * width + col] = new_tuple
        m_row += 1
    
    index = len(metadata) - 1
    while metadata[index] == 0:
        metadata.pop(index)
        index -= 1
    
    current_index = 0
    last_changed = 0
    next = 0
    
    #print(metadata)
    
    #undo changes applied by o_dash
    for row in range(height):
        for col in range(width):
            position = row * width + col
            
            red, green, blue = img[position]
            
            #if this is a changed pixel
            if position == (last_changed + next) and current_index + 3 < len(metadata):
            
                #print("changing index " + str(position))
                nextr, oldr, newr = metadata[current_index]
                nextb, oldg, newg = metadata[current_index + 1]
                nextg, oldb, newb = metadata[current_index + 2]
                
                if red == newr and blue == newb and green == newg:
                    #print("change index of metadata: " + str(current_index))
                    red = oldr
                    green = oldg
                    blue = oldb
                    current_index += 3
                    last_changed = position
                    next, old, new = metadata[current_index]
                else:
                    next += 1
                
            #add pixel back to image
            rgb = (red, green, blue)
            original_image[position] = rgb
    
    #create image in right order
    original_image = np.reshape(original_image, (height, width, 3))
    print(np.shape(original_image))
    img2 = Image.fromarray(original_image.astype(np.uint8))
    img2.save("original_image(ira).png")
            

#MAIN OPERATION
source_folder = "/mnt/c/Users/iraab/source/filters/public/static/images/full_sized_original/"
target_folder = "/mnt/c/Users/iraab/source/filters/public/static/images/full_sized_filtered/"
dest_folder = "/mnt/c/Users/iraab/source/filters/public/static/images/filtered/"

source_length = len(source_folder)

#for-loop for processing
#+ "\\*.jpg"
for filename in os.listdir(source_folder):
    #set filter name manually
    filter = "_cubism"
    #filter = "_oilify"
    
    print(filename)
    
    total = len(filename)
    img_name = filename[:total-4]
    extension = filename[len(img_name):]
    
    target_name = target_folder + img_name + filter + extension
    
    destination = dest_folder + img_name + filter
    
    # Open the image (potentially make it accept stream of data as opposed to image url)
    img1 = Image.open(source_folder + filename)
    img2 = Image.open(target_name)

    # Get the width and height of the image
    width, height = img1.size
    print(str(width) + " pixels(width) by " + str(height) + " pixels(height)")
    thumb_width = 200
    thumb_height = 113

    # Define the size of the block
    block_x = 10
    block_y = 10
    block_size = block_x * block_y

    print("calculated num pixels: " + str(width * height))

    #ENCRYPT
    diff_matrix = blockwise_difference(img1.getdata(), img2.getdata(), block_y, block_x, block_size)

    #print(diff_matrix)
    #print(len(diff_matrix))

    generate_o_dash(img1.getdata(), img2.getdata(), width, height, block_x, block_y, diff_matrix, destination)


    '''#DECRYPT
    img3 = Image.open("modified_image(ira).png")

    undo_o_dash(img3.getdata(), width, height - 1)'''
