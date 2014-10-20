/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Creator/VertexArrayObjectCreator.hh>
#include <ACGL/OpenGL/Data/GeometryDataLoadStore.hh>

#ifdef ACGL_SUPPORT_VAO

using namespace ACGL;
using namespace ACGL::OpenGL;

VertexArrayObjectCreator::VertexArrayObjectCreator(const std::string &_filename)
:   Resource::SingleFileBasedCreator<VertexArrayObject>(_filename, Base::Settings::the()->getFullGeometryPath())
{
}

VertexArrayObjectCreator::VertexArrayObjectCreator(const char *_filename)
:   Resource::SingleFileBasedCreator<VertexArrayObject>(std::string(_filename), Base::Settings::the()->getFullGeometryPath())
{
}

bool VertexArrayObjectCreator::load(SharedVertexArrayObject &_vao)
{
    SharedGeometryData tempData = loadGeometryData( getFullFilePath() );

    if(!tempData)
        return false;

    SharedArrayBuffer sab = SharedArrayBuffer( new ArrayBuffer() );
    for ( uint_t i = 0; i < tempData->mAttributes.size(); ++i ) {
        sab->defineAttribute( tempData->mAttributes[i] );
    }
    sab->setDataElements( tempData->getSize()/tempData->getStrideSize(), tempData->getData() );

    _vao->detachAllAttributes();
    _vao->attachAllAttributes(sab);

    // merge the mappings of the new VAO into the linked LocationMappings object, if present
    if(mAttributeLocations)
    {
        _vao->setAttributeLocations(mAttributeLocations);
        SharedLocationMappings vaoAttributeLocations = _vao->getAttributeLocations();
        mAttributeLocations->addLocations(vaoAttributeLocations);
        _vao->setAttributeLocations(mAttributeLocations);
    }

    return true;
}

SharedVertexArrayObject VertexArrayObjectCreator::create()
{
    updateFileModificationTime();

    SharedVertexArrayObject vao(new VertexArrayObject);

    if(!load(vao))
        return SharedVertexArrayObject();

    return vao;
}

bool VertexArrayObjectCreator::update(SharedVertexArrayObject &vao)
{
    if(fileIsUpToDate())
        return false;

    if(!load(vao))
        return false;

    updateFileModificationTime();
    return true;
}

#endif // #ifdef ACGL_SUPPORT_VAO

