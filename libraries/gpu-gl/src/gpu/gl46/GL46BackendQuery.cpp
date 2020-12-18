//
//  GL46BackendQuery.cpp
//  libraries/gpu/src/gpu
//
//  Created by Sam Gateau on 7/7/2015.
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "GL46Backend.h"

#include <gpu/gl/GLQuery.h>

namespace gpu { namespace gl46 { 

class GL46Query : public gpu::gl::GLQuery {
    using Parent = gpu::gl::GLQuery;
public:
    static GLuint allocateQuery() {
        GLuint result;
        glCreateQueries(GL_TIMESTAMP, 1, &result);
        return result;
    }

    GL46Query(const std::weak_ptr<gl::GLBackend>& backend, const Query& query)
        : Parent(backend, query, allocateQuery(), allocateQuery()) {
    }
};

gl::GLQuery* GL46Backend::syncGPUObject(const Query& query) {
    return GL46Query::sync<GL46Query>(*this, query);
}

GLuint GL46Backend::getQueryID(const QueryPointer& query) {
    return GL46Query::getId<GL46Query>(*this, query);
}

} }