/**
 * \file
 * \author Rudy Castan
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "VertexArray.hpp"
#include "GL.hpp"

namespace OpenGL
{
    /**
     * \brief Creates and configures a Vertex Array Object (VAO) with multiple vertex buffers and optional index buffer
     *
     * This function sets up a complete VAO by:
     * 1. Generating a new VAO handle
     * 2. Binding vertex buffers and configuring their attribute layouts
     * 3. Setting up vertex attribute pointers for each attribute (position, color, texture coords, etc.)
     * 4. Handling both regular floating-point and integer vertex attributes
     * 5. Configuring instanced rendering divisors if needed
     * 6. Optionally binding an index buffer for indexed drawing
     *
     * A VAO encapsulates all the vertex attribute state, so once created, you can simply bind
     * the VAO to use all the configured vertex buffers and their layouts for rendering.
     *
     * \param vertices An initializer list of VertexBuffer objects, each containing:
     *                 - buffer_handle: The OpenGL buffer handle containing vertex data
     *                 - buffer_layout: Description of how the data is organized (attributes, stride, offset)
     * \param index_buffer Optional index buffer handle for indexed rendering (0 if not used)
     *
     * \return VertexArrayHandle The OpenGL handle to the created VAO
     *
     * \note The VAO will be unbound (set to 0) before returning to avoid affecting subsequent OpenGL state
     * \note Each vertex attribute will be assigned sequential attribute indices starting from 0
     */
    VertexArrayHandle CreateVertexArrayObject([[maybe_unused]] std::initializer_list<VertexBuffer> vertices, [[maybe_unused]] BufferHandle index_buffer)
    {
        // PSEUDO CODE for CreateVertexArrayObject:
        // 1. Create a new Vertex Array Object (VAO)
        // 2. Bind the VAO to make it active
        // 3. For each vertex buffer:
        //    a. Bind the buffer as GL_ARRAY_BUFFER
        //    b. Calculate the stride (total bytes per vertex)
        //    c. For each attribute in the buffer layout:
        //       - Enable the vertex attribute array
        //       - Set up the vertex attribute pointer (regular or integer)
        //       - Set the vertex attribute divisor for instancing
        // 4. If an index buffer is provided, bind it as GL_ELEMENT_ARRAY_BUFFER
        // 5. Unbind the VAO (bind 0)
        // 6. Return the VAO handle

        // Declare a variable to hold the VAO handle
        VertexArrayHandle vao{};

        GL::GenVertexArrays(1, &vao);
        // Pass 1 for count and the address of vao to store the generated handle
        // Documentation: https://docs.gl/es3/glGenVertexArrays

        GL::BindVertexArray(vao);
        // This makes all subsequent vertex attribute calls affect this VAO
        // Documentation: https://docs.gl/es3/glBindVertexArray

        // Keep track of the current attribute index (starts at 0)
        [[maybe_unused]] GLuint attribute_index = 0;

        // Each VertexBuffer contains a buffer_handle and buffer_layout
        // Use structured binding:
        for (const auto& [buffer_handle, buffer_layout] : vertices)
        {
            // This tells OpenGL which buffer to read vertex data from
            // Documentation: https://docs.gl/es3/glBindBuffer
            GL::BindBuffer(GL_ARRAY_BUFFER, buffer_handle);

            // Loop through all attributes in buffer_layout.Attributes
            // Sum up each attr_type.SizeBytes to get the total stride
            [[maybe_unused]] GLsizei stride = 0;
            for (const auto& attr_type : buffer_layout.Attributes)
            {
                stride += attr_type.SizeBytes;
            }

            // Cast buffer_layout.BufferStartingByteOffset to GLintptr
            [[maybe_unused]] GLintptr offset = 0;
            offset                           = static_cast<GLintptr>(buffer_layout.BufferStartingByteOffset);

            // Use:
            for (Attribute::Type attr_type : buffer_layout.Attributes)
            {
                if (attr_type == Attribute::None)
                {
                    continue;
                }

                // Documentation: https://docs.gl/es3/glEnableVertexAttribArray
                GL::EnableVertexAttribArray(attribute_index);
                const GLenum    gl_type         = attr_type.GLType;
                const GLint     component_count = attr_type.ComponentCount;
                const GLboolean normalized      = attr_type.Normalize;
                const bool      is_integer      = attr_type.IntAttribute;
                const GLuint    divisor         = attr_type.Divisor;

                if (is_integer == true)
                {
                    // If true:
                    //   Use GL::VertexAttribIPointer for integer attributes
                    GL::VertexAttribIPointer(attribute_index, component_count, gl_type, stride, reinterpret_cast<GLvoid*>(offset));
                    //   Parameters: (attribute_index, component_count, gl_type, stride, offset as GLvoid*)
                    //   Documentation: https://docs.gl/es3/glVertexAttribPointer (contains VertexAttribIPointer)
                }
                else
                {
                    // Else:
                    //   Use GL::VertexAttribPointer for float/normalized attributes
                    GL::VertexAttribPointer(attribute_index, component_count, gl_type, normalized, stride, reinterpret_cast<GLvoid*>(offset));
                    //   Parameters: (attribute_index, component_count, gl_type, normalized, stride, offset as GLvoid*)
                    //   Documentation: https://docs.gl/es3/glVertexAttribPointer
                }


                GL::VertexAttribDivisor(attribute_index, divisor);
                // Parameters: (attribute_index, divisor)
                // Documentation: https://docs.gl/es3/glVertexAttribDivisor

                ++attribute_index;

                offset += attr_type.SizeBytes;
            }
        }

        if (index_buffer != 0)
        {
            // If true:
            //   Bind the index buffer as GL_ELEMENT_ARRAY_BUFFER
            GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
            //   Documentation: https://docs.gl/es3/glBindBuffer
        }


        GL::BindVertexArray(0);
        // This ensures we don't accidentally modify this VAO later
        // Documentation: https://docs.gl/es3/glBindVertexArray

        return vao;
    }

    VertexArrayHandle CreateVertexArrayObject(VertexBuffer vertices, BufferHandle index_buffer)
    {
        return CreateVertexArrayObject({ vertices }, index_buffer);
    }

}
