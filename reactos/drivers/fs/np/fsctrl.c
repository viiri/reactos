/* $Id: fsctrl.c,v 1.4 2001/06/12 12:35:04 ekohl Exp $
 *
 * COPYRIGHT:  See COPYING in the top level directory
 * PROJECT:    ReactOS kernel
 * FILE:       services/fs/np/fsctrl.c
 * PURPOSE:    Named pipe filesystem
 * PROGRAMMER: David Welch <welch@cwcom.net>
 *             Eric Kohl <ekohl@rz-online.de>
 */

/* INCLUDES ******************************************************************/

#include <ddk/ntddk.h>
#include "npfs.h"

//#define NDEBUG
#include <debug.h>


/* FUNCTIONS *****************************************************************/

static NTSTATUS
NpfsConnectPipe(PNPFS_FCB Fcb)
{
   NTSTATUS Status;

   DPRINT("Waiting for connection...\n");

   Status = KeWaitForSingleObject(&Fcb->ConnectEvent,
				  UserRequest,
				  KernelMode,
				  FALSE,
				  NULL);

   DPRINT("Finished waiting! Status: %x\n", Status);

   DPRINT("Client Fcb: %p\n", Fcb->OtherSide);


   return STATUS_SUCCESS;
}


static NTSTATUS
NpfsDisconnectPipe(PNPFS_FCB Fcb)
{

   return STATUS_SUCCESS;
}


static NTSTATUS
NpfsWaitPipe(PNPFS_FCB Fcb)
{

   return STATUS_SUCCESS;
}


NTSTATUS STDCALL
NpfsFileSystemControl(PDEVICE_OBJECT DeviceObject,
		      PIRP Irp)
{
   PIO_STACK_LOCATION IoStack;
   PFILE_OBJECT FileObject;
   NTSTATUS Status;
   PNPFS_DEVICE_EXTENSION DeviceExt;
   PNPFS_PIPE Pipe;
   PNPFS_FCB Fcb;
   
   DPRINT("NpfsFileSystemContol(DeviceObject %p Irp %p)\n", DeviceObject, Irp);
   
   DeviceExt = (PNPFS_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
   IoStack = IoGetCurrentIrpStackLocation(Irp);
   FileObject = IoStack->FileObject;
   DPRINT("FileObject: %p\n", FileObject);
   Fcb = FileObject->FsContext;
   Pipe = Fcb->Pipe;
   
   switch (IoStack->Parameters.FileSystemControl.IoControlCode)
     {
      case FSCTL_PIPE_LISTEN:
	DPRINT("Connecting pipe %wZ\n", &Pipe->PipeName);
	Status = NpfsConnectPipe(Fcb);
	break;

      case FSCTL_PIPE_DISCONNECT:
	DPRINT("Disconnecting pipe %wZ\n", &Pipe->PipeName);
	Status = NpfsDisconnectPipe(Fcb);
	break;

      case FSCTL_PIPE_WAIT:
	DPRINT("Waiting for pipe %wZ\n", &Pipe->PipeName);
	Status = NpfsWaitPipe(Fcb);
	break;

      default:
        DPRINT("IoControlCode: %x\n", IoStack->Parameters.FileSystemControl.IoControlCode)
        Status = STATUS_UNSUCCESSFUL;
     }
   
   Irp->IoStatus.Status = Status;
   Irp->IoStatus.Information = 0;
   
   IoCompleteRequest(Irp, IO_NO_INCREMENT);
   
   return(Status);
}

/* EOF */
