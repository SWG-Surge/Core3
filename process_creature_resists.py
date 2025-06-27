#!/usr/bin/env python3
"""
Script to modify creature resist values for creatures with level 200 or higher.
Requirements:
1. Cap all resist values (indexes 1-9) at 185
2. Replace -1 values with 125
3. Round all resist values to nearest multiple of 5
4. Scale lightsaber resist (index 9) by level: 125 at CL200 to 140 at CL350
"""

import os
import re
import glob
import math

def round_to_nearest_5(value):
    """Round a value to the nearest multiple of 5."""
    return round(value / 5) * 5

def calculate_lightsaber_resist(level):
    """Calculate lightsaber resist based on level (125 at CL200 to 140 at CL350)."""
    if level < 200:
        return 125
    elif level > 350:
        return 140
    else:
        # Linear interpolation: 125 at CL200, 140 at CL350
        # Formula: 125 + (level - 200) * (140 - 125) / (350 - 200)
        # Simplified: 125 + (level - 200) * 15 / 150 = 125 + (level - 200) * 0.1
        resist = 125 + (level - 200) * 0.1
        return round_to_nearest_5(resist)

def process_resists_array(resists_str, level):
    """Process the resists array string and return the modified version."""
    # Extract the array values
    match = re.match(r'resists\s*=\s*\{([^}]+)\}', resists_str)
    if not match:
        return resists_str
    
    values_str = match.group(1)
    # Split by comma and clean up whitespace
    values = [v.strip() for v in values_str.split(',')]
    
    if len(values) != 9:
        print(f"Warning: Expected 9 resist values, found {len(values)}")
        return resists_str
    
    # Process each value
    new_values = []
    for i, value in enumerate(values):
        try:
            # Convert to integer
            resist_value = int(value)
            
            # Apply rules
            if resist_value == -1:
                # Replace -1 with 125
                resist_value = 125
            elif resist_value > 185:
                # Cap at 185
                resist_value = 185
            
            # Round to nearest multiple of 5
            resist_value = round_to_nearest_5(resist_value)
            
            # Special handling for lightsaber resist (index 8, which is the 9th element)
            if i == 8:  # Index 8 is the 9th element (lightsaber)
                resist_value = calculate_lightsaber_resist(level)
            
            new_values.append(str(resist_value))
            
        except ValueError:
            print(f"Warning: Could not parse resist value '{value}'")
            new_values.append(value)
    
    # Reconstruct the resists array
    new_resists_str = f"resists = {{{', '.join(new_values)}}}"
    return new_resists_str

def process_creature_file(file_path):
    """Process a single creature file."""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check if this is a creature file with level
        level_match = re.search(r'level\s*=\s*(\d+)', content)
        if not level_match:
            return False
        
        level = int(level_match.group(1))
        
        # Only process files with level 200 or higher
        if level < 200:
            return False
        
        # Find and process the resists array
        resists_match = re.search(r'resists\s*=\s*\{[^}]+\}', content)
        if not resists_match:
            print(f"Warning: No resists array found in {file_path}")
            return False
        
        original_resists = resists_match.group(0)
        new_resists = process_resists_array(original_resists, level)
        
        # Replace the resists array in the content
        new_content = content.replace(original_resists, new_resists)
        
        # Write the modified content back to the file
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        
        print(f"Processed {file_path} (Level {level}): {original_resists} -> {new_resists}")
        return True
        
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        return False

def main():
    """Main function to process all creature files."""
    # Find all Lua files in the mobile directories
    mobile_dirs = [
        "MMOCoreORB/bin/scripts/mobile",
        "MMOCoreORB/bin/scripts/object/mobile"
    ]
    
    processed_count = 0
    total_files = 0
    
    for mobile_dir in mobile_dirs:
        if not os.path.exists(mobile_dir):
            print(f"Directory not found: {mobile_dir}")
            continue
        
        # Find all .lua files recursively
        lua_files = glob.glob(os.path.join(mobile_dir, "**/*.lua"), recursive=True)
        
        for lua_file in lua_files:
            total_files += 1
            if process_creature_file(lua_file):
                processed_count += 1
    
    print(f"\nProcessing complete!")
    print(f"Total files scanned: {total_files}")
    print(f"Files processed (level 200+): {processed_count}")

if __name__ == "__main__":
    main() 