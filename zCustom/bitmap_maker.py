import pygame
import sys

# Initialize Pygame
pygame.init()

# Set up colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
GRAY = (200, 200, 200)
RED = (255, 0, 0)

# Set bitmap dimensions
width = 16
height = 16

# Set up the display
CELL_SIZE = 30
MARGIN = 1
BUTTON_HEIGHT = 40
WINDOW_SIZE = (width * (CELL_SIZE + MARGIN) + MARGIN, height * (CELL_SIZE + MARGIN) + MARGIN + BUTTON_HEIGHT)
screen = pygame.display.set_mode(WINDOW_SIZE)
pygame.display.set_caption("16x16 Bitmap Designer")

# Create the grid
grid = [[0 for _ in range(width)] for _ in range(height)]

# Create finish button
button_rect = pygame.Rect(MARGIN, height * (CELL_SIZE + MARGIN) + MARGIN, WINDOW_SIZE[0] - 2*MARGIN, BUTTON_HEIGHT - MARGIN)
button_font = pygame.font.Font(None, 36)
button_text = button_font.render("Finish", True, WHITE)
button_text_rect = button_text.get_rect(center=button_rect.center)

# Main game loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            # Get mouse position
            pos = pygame.mouse.get_pos()

            # Check if finish button was clicked
            if button_rect.collidepoint(pos):
                print("Bitmap data:")
                for row in grid:
                    byte = 0
                    for i, bit in enumerate(row):
                        byte |= bit << (7 - i % 8)
                        if (i + 1) % 8 == 0 or i == len(row) - 1:
                            print(f"0x{byte:02X}, ", end="")
                            byte = 0
                    print()
                running = False
            else:
                # Calculate which cell was clicked
                column = pos[0] // (CELL_SIZE + MARGIN)
                row = pos[1] // (CELL_SIZE + MARGIN)

                # Toggle the cell if it's within the grid
                if 0 <= row < height and 0 <= column < width:
                    grid[row][column] = 1 - grid[row][column]

    # Clear the screen
    screen.fill(GRAY)

    # Draw the grid
    for row in range(height):
        for column in range(width):
            color = BLACK if grid[row][column] else WHITE
            pygame.draw.rect(screen, color,
                             [(CELL_SIZE + MARGIN) * column + MARGIN,
                              (CELL_SIZE + MARGIN) * row + MARGIN,
                              CELL_SIZE,
                              CELL_SIZE])

    # Draw the finish button
    pygame.draw.rect(screen, RED, button_rect)
    screen.blit(button_text, button_text_rect)

    # Update the display
    pygame.display.flip()

# Quit Pygame
pygame.quit()
sys.exit()
